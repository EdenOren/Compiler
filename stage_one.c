#include "stage_one.h"
/*
 * Function:  stage_one 
 * --------------------
 * Converting stream into a sequence of tokens & analyzing them
 * file_p: pointer to given file
 * file_n: file name
 * sym_head: symbol table
 * inst_head: instructions table
 * curr_address: address counter
 * inst_image: instruction image (machine code)
 * data_image: data image (machine code)
 * returns: returns 0 if no errors were found, 1 otherwise
 */
int stage_one(FILE *file_p, const char * file_n, symbol ** sym_head, instruction * inst_head,
address ** curr_address,  code_t * inst_image, code_t * data_image)
{
    char original_line [MAX_LINE_LENGTH + BUFF] = {0};
    char cmd_name [MAX_LABEL_LENGTH] = {0}; /*command name for error prinitng*/
    int line_num = 1, error_flag = 0; /*line num for error prinitng, flag 1 upon encountered error*/
    enum ERROR_NUM error = 0; 
    enum COMMAND_TYPE cmd_type = 0;

    while (fgets(original_line, MAX_LINE_LENGTH + BUFF, file_p))
    {
        /*get current cmd from first word of line*/
        cmd_type = preliminary_analysis(file_p, file_n, original_line, line_num, &error_flag);
        get_command(original_line, cmd_name, NOT_CLASSIFIED); /*get first word of line*/
        
        if  (cmd_type != NOT_CLASSIFIED) /*line need further analysis*/
        {
            if ((error = analyse_type(cmd_type, original_line, sym_head, inst_head, curr_address, inst_image, data_image)))
            {
                print_error(error, file_n, line_num, cmd_name); /*print error to stdout*/
                error_flag = 1; /*flag error*/
            }
            memset(cmd_name, 0, MAX_LABEL_LENGTH);
        }
        line_num++;
    } /*end while*/
    if (error_flag) /*prevents proceeding to stage two*/
        return 1;
    return 0;
} /*end of stage one*/

/*
 * Function:  preliminary_analysis 
 * --------------------
 * Validates stream rows, classfies their type based on first word
 * file_p: pointer to given file
 * file_n: file name
 * original_line: stream line 
 * line_num: line number
 * error_flag: flag 1 upon occured errors
 * returns: returns COMMAND TYPE (>0) for further analysis, NOT CLASSIFIED (0) otherwise
 */
enum COMMAND_TYPE preliminary_analysis(FILE *file_p, const char * file_n, char original_line[],
int line_num, int * error_flag) 
{
    enum ERROR_NUM error = 0; 
    char temp_line [MAX_LINE_LENGTH] = {0}; /* line copy to be cut to tokens */
    char * token, cmd_string[MAX_LABEL_LENGTH] = {0};

    /* handle line overflow */
    if (last_printable_char(original_line) > MAX_LINE_LENGTH - 1) 
    {
        char ch;
        while ((ch = getc(file_p)) != '\n' && !feof(file_p)); /*flush extra chars. */
        
        *error_flag = 1; /*flag error*/
        print_error(LINE_INVALID_LENGTH, file_n, line_num, NULL); /*print error to stdout*/
        return NOT_CLASSIFIED;
    }
    memcpy(temp_line, original_line, MAX_LINE_LENGTH);
    if (is_comment_or_empty_line(temp_line))
        return NOT_CLASSIFIED;

    token = strtok(temp_line, "\n\t\r "); /*get first token*/

    if (IS_INSTRUCTION(token))
        return INSTRUCTION_CMD;

    if (IS_DIRECTIVE(token))
        return DIRECTIVE_CMD;
        
    if (!(error = is_label(token, YES_COLON))) /*is a label */
        return LABEL_CMD;

    get_command(original_line, cmd_string, LABEL_CMD);
    if (IS_ENTRY_OR_EXTERN(cmd_string)) /*entry & extern ignores preceding label, even if invalid*/
    {
        return LABEL_CMD;
    } else { /*unknown command / ilegal label*/
        *error_flag = 1;
        print_error(error, file_n, line_num, token);
        return NOT_CLASSIFIED;    
    } 
} /*end of preliminary_analysis */

/*
 * Function:  analyse_type 
 * --------------------
 * call analyse function based on given type
 * cmd_type: type of command (directive, instruction, label)
 * this_line: currently analysed line
 * sym_head: symbol table
 * inst_head: instructions table
 * curr_address: address counter
 * inst_image: instruction image (machine code)
 * data_image: data image (machine code)
 * returns: returns error num (if occured) from relevant called function, 0 (OK) otherwise
 */
enum ERROR_NUM analyse_type(enum COMMAND_TYPE cmd_type, char this_line[], symbol ** sym_head,
    instruction * inst_head, address ** curr_address, code_t * inst_image, code_t * data_image) 
{
    enum ERROR_NUM error = clear_and_examine_commas(this_line, cmd_type);
    RETURN_IF_ERROR(error);

    switch(cmd_type) {
        case INSTRUCTION_CMD: 
            return analyse_instruction(this_line, curr_address, sym_head, inst_head, inst_image);
            break;
        case DIRECTIVE_CMD:
            return analyse_directive(this_line, curr_address, sym_head, data_image);
            break;
        case LABEL_CMD:  
            return analyse_label(this_line, curr_address, sym_head, inst_head, inst_image, data_image);
            break;
        case NOT_CLASSIFIED:
            return OK;
            break;
    } /* end of switch cmd */
    return OK;
} /*end of analyse type */

/*
 * Function:  analyse_instruction 
 * --------------------
 * analyse instruction line, add to instruction count address and instruction image
 * this_line: currently analysed line
 * curr_address: address counter
 * sym_head: symbol table
 * inst_head: instructions table
 * inst_image: instruction image (machine code)
 * returns: returns 0 (OK) if passed error checks,  error num otherwise
 */
enum ERROR_NUM analyse_instruction(char this_line[], address ** curr_address, 
    symbol ** sym_head, instruction * inst_head, code_t * inst_image) 
{
    unsigned long opcode_value = 0, code_value = 0, param_array[THREE] = {0};
    char * token = NULL;
    enum ERROR_NUM error = OK;

    token = strtok(this_line, "\n\t\r ");
    if (token == NULL)
        return MISSING_PARAM;

    code_value = get_instruction(inst_head, token) -> value; /*get instruction bit code*/
    opcode_value = btoi(code_value, OPCODE_LOCATION, ADDRESS_LENGTH); /*get opcode value (decimal)*/
    token = strtok(NULL, "\n\t\r ");

    if (OPC_R(opcode_value))
    {
        error = analyse_ins_r(opcode_value, token, param_array);
        RETURN_IF_ERROR(error);
    }
    if (OPC_I(opcode_value))
    {
        error = analyse_ins_i(opcode_value, token, param_array);
        RETURN_IF_ERROR(error);
    }

    if (OPC_J(opcode_value)) 
    {
        error = analyse_ins_j(opcode_value, token, param_array);
        RETURN_IF_ERROR(error);
    }
    
    /*add parameters to iamge*/
    update_code_parameters(param_array, &code_value, opcode_value);
    add_to_image(inst_image, get_address(curr_address, IS_IC), code_value, IC_ADDRESS_SIZE);
    set_address(curr_address, IC_ADDRESS_SIZE, IS_IC); /* add to address */

    return OK;
} /* end analyse instruction */ 

/*
 * Function:  analyse_directive 
 * --------------------
 * analyse directive line, add to data count address and data image
 * this_line: currently analysed line
 * curr_address: address counter
 * sym_head: symbol table
 * inst_image: instruction image (machine code)
 * returns: returns 0 (OK) if passed error checks,  error num otherwise
 */
enum ERROR_NUM analyse_directive(char this_line[], address ** curr_address, symbol ** sym_head, code_t * data_image) 
{
    enum ERROR_NUM error = OK;
    char * token, command_name[MAX_LABEL_LENGTH] = {0}, temp[MAX_LINE_LENGTH] = {0};
    int data_size = 0, i = 0;
    unsigned long encoding = 0;

    strcpy(temp, this_line);
    get_command(temp, command_name, !LABEL_CMD);
    token = strtok(temp, "\n\t\r ");
    token = strtok(NULL, "\n\t\r ");

    /* directives: db, dh, dw */
    if (IS_DATA_DIRECTIVE(command_name))
    {
        switch (command_name[strlen(command_name)-1]) /*determine data size by command*/
        {
            case 'b': data_size = DB; break;
            case 'h': data_size = DH; break;
            case 'w': data_size = DW; break;
        }
        if (token == NULL)
            return MISSING_PARAM;
        /*validate all number parameters. exceeding data size is valid*/
        while ((token != NULL) && !(error = is_valid_num(token))) 
        {
            int pos = 0;
            int num = get_number(token, &pos);
            int range_flag = is_out_of_range(num, data_size); /*1 if out of range, 0 otherwise*/
            encoding = itob(num, data_size, range_flag); /* num to bit code*/

            token = strtok(NULL, "\n\t\r ");

            /*add each validated num to data image*/
            add_to_image(data_image, get_address(curr_address, IS_DC), encoding, data_size);
            set_address(curr_address, data_size , IS_DC); /* add data_size to address */
        }
    }
    /* directive: asciz */
    if (IS_ASCIZ(command_name))
    {
        if (!(error = string_analysis(this_line, &data_size))) /*check parameters */
        {
            int str_start = get_quote(0, strlen(this_line), this_line) + 1; /*get to index of first char in string*/
            int char_size = 1; /*each char consist of 1 byte*/
            int range_flag = 0; /*char always in range in byte*/

            for (i = 0; i < data_size - 1; i++) /*add each char to data image*/
            {
                encoding = itob(this_line[str_start + i], char_size, range_flag);
                add_to_image(data_image, get_address(curr_address, IS_DC), encoding, char_size);
                set_address(curr_address, char_size , IS_DC); /* add to address */
            }
            set_address(curr_address, char_size , IS_DC); /* add \0 to address */
        }
    }
    /* directives: entry or extern */
    if (IS_ENTRY_OR_EXTERN(command_name))
    {
        symbol * existing_symbol = get_symbol(sym_head, token);; /*holds existing ptr to symbol if exists */
        error = is_label(token, NO_COLON); /*check valid name of extern/entry*/
        RETURN_IF_ERROR(error);
        /*add extern to symbol table */
        if (IS_EXTERN(command_name))
        {
            error = add_extern_symbol(token, existing_symbol, sym_head, curr_address);
            RETURN_IF_ERROR(error);
        }
        token = strtok(NULL, "\n\t\r ");
        /* check for extraneous operand in extern/entry*/
        if (token != NULL) 
            return EXTRANEOUS_OPERAND;
    }
    RETURN_IF_ERROR(error);
    return OK;
} /*end of analyse directive */

/*
 * Function:  analyse_label 
 * --------------------
 * analyse line starting with label, add to symbol table, and analyse remaining line
 * this_line: currently analysed line
 * curr_address: address counter
 * sym_head: symbol table
 * inst_head: instructions table
 * inst_image: instruction image (machine code)
 * data_image: data image (machine code)
 * returns: returns 0 (OK) if passed error checks,  error num otherwise, from relevant function calls
 */
enum ERROR_NUM analyse_label(char this_line[], address ** curr_address, symbol ** sym_head,
    instruction * inst_head, code_t * inst_image, code_t * data_image) 
{
    char label_name[MAX_LABEL_LENGTH] = {0}, command_name[MAX_LABEL_LENGTH] = {0};
    symbol * existing_symbol = NULL; /*holds existing symbol ptr if exists */

    get_label(this_line, label_name);
    get_command(this_line, command_name, LABEL_CMD);
    existing_symbol = get_symbol(sym_head, label_name);

    if (command_name == NULL)
            return MISSING_PARAM;
    /* check if extern/entry to ignore label*/
    if (IS_DIRECTIVE(command_name))
    {
        if (IS_ENTRY_OR_EXTERN(command_name)) /*entry & extern ignores preceding label*/
        {
            return analyse_directive(this_line + (strlen(label_name) + 1), curr_address, sym_head, data_image);
        } else { /* case directive is not extern or entry */
            if (existing_symbol != NULL) /*symbol exists*/
            {
                return DUPLICATE_SYMBOL;
            } else { /*create new symbol with current address and data attribute */
                add_symbol(sym_head, label_name, get_address(curr_address, IS_DC));
                set_symbol(sym_head, label_name, A_DATA, SET_ATTRIBUTE);
            }
            return analyse_directive(this_line + (strlen(label_name) + 1), curr_address, sym_head, data_image);
        }
    } else if (IS_INSTRUCTION(command_name)) { 
        if (existing_symbol != NULL) /*symbol exists */
        {
            return DUPLICATE_SYMBOL;
        } else { /*create new symbol with current address and data attribute */
            add_symbol(sym_head, label_name, get_address(curr_address, IS_IC));
            set_symbol(sym_head, label_name, A_INST, SET_ATTRIBUTE);
        }
        return analyse_instruction(this_line + (strlen(label_name) + 1), curr_address, sym_head, inst_head, inst_image);
    } else {
        return COMMAND_UNRECOGNIZED;
    }
    return OK;
} /*end of analyse label */

