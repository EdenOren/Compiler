/*
 * This program is a compiler of assembler language (as defined in Maman 14)
 * that processes statements from given file parameters in two stages
 * if all stages are successfull, creates output accordingly
 * --------------------------------------------------------------------------------------------------------
 * Author: Eden Oren
 * Date: 12th august, 2021
 */
#include "assembler.h"

/*
 * function: main
 * --------------------------------
 * read .as files and compile them, returning errors if occure
 * and creates object, extern and entry files if given file is valid
 */
int main(int argc, char * argv[]) {
    FILE * file_p = NULL; /*pointer to file */
    char file_n [MAX_FILE_NAME]; /*file name */
    instruction * inst_head;
    int i;
    
    if (argument_exist(argc)) /*no arguments given */
    {
        exit (1);
    }
    inst_head = create_instructions_table(); /*create instruction table */
    for (i = 1; i < argc; i++) /* loop for each file */
    {
        memset(file_n, 0, MAX_FILE_NAME);
        memcpy(file_n, argv[i], MAX_FILE_NAME);
        if ((file_p = get_file(file_n)) != NULL) /*file open & error check */
        {
            /*symbol table, address table and instruction code & data code variables*/
            symbol * sym_head_ptr = NULL;
            symbol ** sym_head_pp = &sym_head_ptr;
            address * address_ptr = NULL;
            address ** address_pp = &address_ptr;
            code_t * instruction_image = NULL, * data_image = NULL;
            code_t ** i_image = &instruction_image, ** d_image = &data_image;
            
            /*initializers*/
            init_addresses(address_pp);
            init_code_table(i_image);
            init_code_table(d_image);
            
            if (!stage_one(file_p, file_n, sym_head_pp, inst_head, address_pp, *i_image, *d_image)) /*stage one*/
            {
                /*set ICF & DCF - final insturction & data count values*/
                int ICF = get_address(address_pp, IS_IC);
                int DCF = get_address(address_pp, IS_DC);

                /*prepare current file, symbol table and code image for stage two*/
                update_symbol_addresses(sym_head_pp, ICF);       /* update with final instruction count */
                create_code_image(*i_image, *d_image, DCF, ICF); /* set code image: instructions + data images */
                free(*d_image);                                  /* data image was moved to the code image */
                if (fseek(file_p, 0, SEEK_SET))                  /* get to file start */
                    print_fundamental_error(FILE_READING_ERROR, file_n);

                if (!stage_two(file_p, file_n, sym_head_pp, *i_image)) /*stage two*/
                {
                    create_output(sym_head_pp, file_n, *i_image, ICF, DCF, (*address_pp) -> data_arr); 
                }
            }
            close_and_clear(file_p, sym_head_pp, address_pp, *i_image);
        }
    } /*end for */
    delete_instructions_table(inst_head);
    return 0;
} /* end main */

/*close current file and free allocated memory */
void close_and_clear(FILE * file_p,  symbol ** sym_head, address ** curr_address, code_t * i_image) {
    fclose (file_p);
    delete_symbol_table(sym_head);
    delete_data_size_table(&(*curr_address) -> data_arr);
    free(*curr_address);
    free(i_image);
}