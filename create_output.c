#include "create_output.h"

/*
 * Function:  create_output 
 * --------------------
 * create object file and extern, entry files if their values exist
 * sym_head: symbol table
 * file_n: original file name
 * code_image: program machine code
 * ICF: final instruction count
 * DCF: final data count
 * data_size_array: size of data for each data directive
 */
void create_output(symbol ** sym_head, char * file_n, code_t * code_image, 
int ICF, int DCF, data_array * data_size_array) 
{
    int extern_flag = 0, entry_flag = 0; /*flag entry/extern for output files*/
    symbol * current_symbol = *sym_head;
    
    /*check if extern and entry symbols exist and change flag*/
    while (current_symbol != NULL)
    {
        if (current_symbol -> attrib[A_EXTERN] == 1 && current_symbol -> address != 0)
            extern_flag = 1;
        if (current_symbol -> attrib[A_ENTRY] == 1)
            entry_flag = 1;
        current_symbol = current_symbol -> next;
        if (extern_flag && entry_flag) /*prevent needless loop through symbol table*/
            break;
    }
    /*create output files*/
    object_output(file_n, code_image, ICF, DCF, data_size_array);
    if (extern_flag)
        extern_entry_output(file_n, sym_head, A_EXTERN);
    if (entry_flag)
        extern_entry_output(file_n, sym_head, A_ENTRY); 
} /*end of create_output*/

/*
 * Function:  object_output 
 * --------------------
 * create object file and print hex code image in ascending order
 * fname: new file name
 * code_image: machine code to print as hex
 * ICF: final instruction count
 * DCF: final data count
 * data_size_array: size of data for each data directive
 */
void object_output(char * fname, code_t * code_image, int ICF, int DCF, data_array * data_size_array) {
    FILE * file_p = get_output_file_name(fname, OBJ_OUT);
    int current_index = IC_MIN_ADDRESS, dsize = DATA_SIZE_END; /*dsize is current size of data directive*/
    int last_print_index = IC_MIN_ADDRESS - IC_ADDRESS_SIZE;

    fprintf(file_p, "%11d %d", ICF - IC_MIN_ADDRESS, DCF); /*print headline*/    
    dsize = get_dsize(current_index, dsize, ICF, &data_size_array);  /*get first command data size*/

    while (current_index < (ICF + DCF))
    {
        int print_index = dsize - 1; /*set print_index to the first byte of the current command code*/
        while (print_index >= 0)
        {
            if (current_index + print_index < ICF + DCF)
            {
                print_new_row(file_p, current_index, print_index, &last_print_index, dsize); /*check if required to print row headline*/
                fprintf(file_p, "%X", btoi(code_image[current_index + print_index].value, BITS_IN_BYTE/BINARY, BITS_IN_BYTE)); /*bits 4 - 7*/
                fprintf(file_p, "%X ", btoi(code_image[current_index + print_index].value, 0, BITS_IN_BYTE/BINARY)); /*bits 0 - 3*/
            }
            print_index--; /*get next byte*/
        }
        current_index += dsize; /*add total printed data size to current index */
        dsize = get_dsize(current_index, print_index, ICF, &data_size_array); /*get next command data size*/
    }
    fclose(file_p);
} /*end of object_output*/

/*
 * Function:  extern_entry_output 
 * --------------------
 * create extern or entry file and print symbol name and address
 * fname: new file name
 * sym_head: symbol table
 * type: determine type of file to create
 */
void extern_entry_output(char * fname, symbol ** sym_head, enum SYMBOL_ATTRIBUTE type) {
    symbol * current_symbol = *sym_head;
    enum FILE_OUTPUT_TYPE output_type = (type == A_ENTRY)? ENT_OUT : EXT_OUT;
    FILE * file_p = get_output_file_name(fname, output_type);;

    while (current_symbol != NULL) /* loop symbol table, print by type*/
    {
        if ((current_symbol -> attrib[type] == 1) && (current_symbol -> address != 0)) /* prevent unused symbols (address 0)*/
            fprintf(file_p, "%s %04lu\n", current_symbol->label, current_symbol->address);
        current_symbol = current_symbol -> next;
    }
    fclose(file_p);
} /*end of extern and entry output*/

/*
 * Function:  print_new_row 
 * --------------------
 * check if new row header is needed and print accordingly & update last print index
 * file_p: pointer to file
 * current_index: current command base address
 * print_index: index used to determine if there is remaining data to print from previous command
 * last_print_index: last address header printed to file
 * dsize: current data size in bytes
 */
void print_new_row(FILE * file_p, int current_index, int print_index, int * last_print_index, int dsize) {
    int rem = (current_index + dsize) % IC_ADDRESS_SIZE;

    if (current_index > *last_print_index && (current_index - *last_print_index) >= IC_ADDRESS_SIZE)
    {
        fprintf(file_p, "\n%04d ", *last_print_index + IC_ADDRESS_SIZE);
        *last_print_index += IC_ADDRESS_SIZE;
    } else if ((rem > 0) && (print_index == rem - 1)) /*bigger than 4 in a row*/   
    {
        fprintf(file_p, "\n%04d ", *last_print_index + IC_ADDRESS_SIZE);
        *last_print_index += IC_ADDRESS_SIZE;
    }
}

/*
 * Function:  get_dsize 
 * --------------------
 * check current instruction / directive data size and return it
 * current_index: current command base address
 * print_index: index used to determine if there is remaining data to print from previous command
 * ICF: final instruction count
 * data_size_array: directives data size array
 * return: return data size of next command if last command was fully printed, else returns last command print index
 */
int get_dsize(int current_index, int print_index, int ICF, data_array ** data_size_array) {
    int next_data_size = print_index;
    
    if (current_index < ICF) {
        return (IC_ADDRESS_SIZE);
    } else if (print_index < 0) { /*all previous data was printed*/
        if (*data_size_array != NULL) {
            next_data_size = (*data_size_array) -> data_size;
            *data_size_array = (*data_size_array) -> next;
        } else {
            return DATA_SIZE_END; /*end of data array*/
        }
    }
    return next_data_size;
}

/* add extention to file name, create empty file with that name, and return pointer to it*/
FILE * get_output_file_name(char * fname, enum FILE_OUTPUT_TYPE type) {
    char * c = strrchr(fname, '.'); /*find pointer to extention*/
    if (c == NULL)
    {
        print_fundamental_error(FILE_INVALID_NAME, fname);
        exit(1);
    }
    *c = '\0'; /*remove extention*/

    switch (type) 
    {
        case OBJ_OUT:
            strcat(fname, OBJ_EXT);
            break;
        case EXT_OUT:
            strcat(fname, EXTERN_EXT);
            break;
        case ENT_OUT:
            strcat(fname, ENTRY_EXT);
            break;
    }
   return fopen(fname, "w");
}