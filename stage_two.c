#include "stage_two.h"

/*
 * Function:  stage_two 
 * --------------------
 * Converting stream into a sequence of tokens, for second time
 * completing code image and addional error check and analysis
 * file_p: given file pointer
 * file_n: file name
 * sym_head: symbol table
 * code_image: code image: instructions & data (machine code) 
 * returns: returns 0 if no errors were found, 1 otherwise
 */
int stage_two(FILE *file_p, const char * file_n, symbol ** sym_head, code_t * code_image)
{
    int address_count = IC_MIN_ADDRESS; /*track address for code updates*/
    char original_line [MAX_LINE_LENGTH] = {0};
    char cmd_name [MAX_LABEL_LENGTH] = {0}; /*command name for error prinitng*/
    int line_num = 1, error_flag = 0; /*line num for error prinitng, error flag tracks if error occured during stage two*/
    enum ERROR_NUM error = OK; 

    while (fgets(original_line, MAX_LINE_LENGTH, file_p))
    {
        get_command(original_line, cmd_name, NOT_CLASSIFIED); /*get first word of line*/
        if ((error = final_analysis(original_line, &line_num, sym_head, code_image, &address_count)))
        {
            print_error(error, file_n, line_num, cmd_name);
            line_num++;
            error_flag = 1;
        }
        memset(cmd_name, 0, MAX_LABEL_LENGTH);
    } /*end while*/

    if (error_flag) /*prevents file output*/
        return 1;
    return 0;
} /*end of stage two*/

/*
 * Function:  final_analysis 
 * --------------------
 * handle entry directive, add symbol addresses to code image
 * and check additional errors
 * original_line: line from stream
 * line_num: line number
 * sym_head: symbol table
 * code_image: code image: instructions & data (machine code) 
 * address_count: address counter
 * return OK if no errors occured, error num otherwise
*/
enum ERROR_NUM final_analysis(char original_line[], int * line_num, symbol ** sym_head,
code_t * code_image, int * address_count)
{
    char temp_line [MAX_LINE_LENGTH] = {0}; /* line copy to be cut to tokens */
    char * token = NULL;
    enum ERROR_NUM error = OK;

    if (is_comment_or_empty_line(original_line)) /*is empty or comment */ 
    {
        (*line_num)++;
        return OK;
    }

    clear_and_examine_commas(original_line, get_cmd_type(original_line));
    strcpy(temp_line, original_line);
    token = strtok(temp_line, "\n\t\r "); /*get 1st token*/

    if (!is_label(token, YES_COLON)) /*skip label if exist*/ 
    {
        token = strtok(NULL, "\n\t\r ");
    }
    if (IS_DIRECTIVE(token))
    { 
        if (IS_ENTRY(token)) /*skip directives except entry*/
        {
            token = strtok(NULL, "\n\t\r "); /*get 2nd token - symbol/label*/
            error = add_entry_symbol(token, sym_head);
            RETURN_IF_ERROR(error);
        }
    }
    if (IS_INSTRUCTION(token)) 
    {   
        char * temp = NULL; /*temp string to get symbol name*/
        
        /*get last token in line = label name (in i & j instructions)*/
        temp = strtok(NULL, "\n\t\r ");
        while (temp != NULL) 
        {
            token = temp;
            temp = strtok(NULL, "\n\t\r ");
        }
        error = final_instruction_analysis(token, sym_head, code_image, address_count);
        RETURN_IF_ERROR(error);
    }
    (*line_num)++;
    return OK;
} /*end of final_analysis */

/*
 * Function:  final_instruction_analysis 
 * --------------------
 * add symbol addresses to code image in instructions
 * and check additional errors
 * token: symbol name (for specific instructions)
 * sym_head: symbol table
 * code_image: code image: instructions & data (machine code) 
 * address_count: address counter
 * return OK if no errors occured, error num otherwise
*/
enum ERROR_NUM final_instruction_analysis(char * token, symbol ** sym_head, code_t * code_image, int * address_count) {
    int opcode_value = 0, address_param = 0; /*address_param - relevant value to add to data table*/
    unsigned long encoding = 0; /*machine code from and to data image*/
    symbol * curr_symbol = NULL; 

    get_from_image(code_image, * address_count, &encoding, IC_ADDRESS_SIZE); /*get current image to encoding*/
    opcode_value = btoi(encoding, OPCODE_LOCATION, ADDRESS_LENGTH); /*get opcode value from encoding*/

    if (IS_BRANCHED_CONDITION(opcode_value)) /*add symbol address to branched condition*/
    {
        curr_symbol = get_symbol(sym_head, token);
        if (curr_symbol == NULL)
        {
            * address_count += IC_ADDRESS_SIZE;
            return MISSING_SYMBOL;
        }
        address_param = curr_symbol -> address - * address_count; /*distance between addresses*/

        if (curr_symbol->attrib[A_EXTERN] == 1) /*check symbol is not extern*/
            return SYMBOL_IS_EXTERN;
        if (abs(address_param) > MAX_NUM(SIZE_OF_IMMED_I/BITS_IN_BYTE)) /*check addresses distance in range*/
            return IMMED_OUT_OF_RANGE;
        
        add_code_parameter(address_param, &encoding, 0, SIZE_OF_IMMED_I);
    }
    /*add symbol address to jump (if reg field is 0) or to call & la  */
    if ( ((OPC_JUMP(opcode_value)) && ((encoding >> (OPCODE_LOCATION - 1) & 1) == 0)) || (OPC_CALL_OR_LA(opcode_value)) ) 
    {
        curr_symbol = get_symbol(sym_head, token);
        if (curr_symbol == NULL) /*check symbol exist*/
        {
            * address_count += IC_ADDRESS_SIZE;
            return MISSING_SYMBOL;
        }
        address_param = curr_symbol -> address; /*get parameter to add to image*/

        if (curr_symbol -> attrib[A_EXTERN] == 1) /*case extern - set address of extern for extern output file */
        {
            add_extern_address(sym_head, token, curr_symbol -> address, *address_count);
            address_param = 0; /* code table address for extern always 0*/
        }
        add_code_parameter(address_param, &encoding, 0, SIZE_OF_IMMED_I);
    }
    /*rest of r instruction only add to address count, no further analysis needed*/

    add_to_image(code_image, * address_count, encoding, IC_ADDRESS_SIZE);
    * address_count += IC_ADDRESS_SIZE;
    return OK; 
} /* end of final_instruction_analysis*/

/*
 * Function:  add_entry_symbol 
 * --------------------
 * Adds entry attribute to existing symbol 
 * symbol_name: string symbol name
 * sym_head: symbol table head
 * returns: returns OK (0) if attribute added, error number if symbol is missing or exist as extern
 */
enum ERROR_NUM add_entry_symbol(char * symbol_name, symbol ** sym_head) {
    enum ERROR_NUM error = OK;
    symbol * existing_symbol = get_symbol(sym_head, symbol_name);

    if (existing_symbol == NULL) /*symbol doesnt exist */
    {
        return MISSING_SYMBOL;
    } else if (existing_symbol->attrib[A_EXTERN] == 1) { /* symbol exist as extern*/
            return SYMBOL_AMABIGUOUS;
    }
    if (!(error = is_label(symbol_name, NO_COLON))) /*check symbol is valid label*/ 
    {
        set_symbol(sym_head, symbol_name, A_ENTRY, SET_ATTRIBUTE);
    } else { /* invalid symbol name */
        return error;
    }
    return OK;
} /* end of add_entry_symbol*/

/*
 * Function:  get_cmd_type 
 * --------------------
 * check if line starts with label or not
 * original_line: stream line
 * returns: returns LABEL_CMD if starts with label, NOT_CLASSIFIED otherwise
 */
enum COMMAND_TYPE get_cmd_type(char original_line[])
{
    char temp_line [MAX_LABEL_LENGTH] = {0};
    char * token;

    strncpy(temp_line, original_line, MAX_LABEL_LENGTH);
    token = strtok(temp_line, "\n\t\r ");

    if (!is_label(token, YES_COLON))
    {
        return LABEL_CMD;
    } else {
        return NOT_CLASSIFIED;
    }
} /* end of get_cmd_type*/

/*
 * Function:  add_extern_address 
 * --------------------
 * set address of extern for extern output file
 * sym_head: symbol table head
 * token: extern name
 * symbol_address: current address of symbol in table
 * address_count: address to set to existing / new extern symbol
 */
void add_extern_address(symbol ** sym_head, char * token, int symbol_address, int address_count) {
    if (symbol_address == 0)  { /*add address to extern, first appearance*/
        set_symbol(sym_head, token, address_count, SET_ADDRESS);
    } else { /*add extern with another address, more than 1 appearance*/
        add_symbol(sym_head, token, address_count);
        set_symbol(sym_head, token, A_EXTERN, SET_ATTRIBUTE);
    }
}