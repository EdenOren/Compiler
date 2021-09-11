#include "stage_one_func.h"

/*
 * Function:  analyse_ins_r 
 * --------------------
 * analyse instructions R
 * opcode_value: opcode from image table to determine instruction
 * token: current string parameter from line
 * param_array: empty array to fill with valid parameters to add to image code
 * returns: returns 0 if all numbers are valid, error number otherwise
 */
enum ERROR_NUM analyse_ins_r(unsigned long opcode_value, char * token, unsigned long param_array[]) {
    /* determine amount of regs needed by opcode value*/
    int param_needed = (opcode_value == 0)? THREE : TWO;
    int pos, reg_num; 
    enum ERROR_NUM error = OK;

    /*validate registers and count*/
    while ((token != NULL) && !(error = is_valid_register(token)))
    {
        pos = 1; /*skip $*/
        reg_num = get_number(token, &pos);
        
        param_array[param_needed - 1] = itob(reg_num, R_SIZE, 0); /*convert reg num to bit code and add to param array*/
        token = strtok(NULL, "\n\t\r ");
        param_needed--;
    }
    RETURN_IF_ERROR(error);
    if (param_needed < 0)
        return EXTRANEOUS_OPERAND;
    if (param_needed > 0)
        return MISSING_PARAM;

    return OK;
}

/*
 * Function:  analyse_ins_i 
 * --------------------
 * analyse instructions I
 * opcode_value: opcode from image table to determine instruction
 * token: current string parameter from line
 * param_array: empty array to fill with valid parameters to add to image code
 * returns: returns 0 if all numbers are valid, error number otherwise
 */
enum ERROR_NUM analyse_ins_i(unsigned long opcode_value, char * token, unsigned long param_array[]) {
    int param_count = 0, param_needed = THREE; /*count parameters */
    long param; /*parameter to add to image*/
    int pos;
    enum ERROR_NUM error = OK;

    if (IS_BRANCHED_CONDITION(opcode_value)) /* checks reg, reg, label */ 
    { 
        for (param_count = 0; (param_count < param_needed - 1) && (token != NULL); param_count++)
        {
            pos = 1; /*pos 0 = $*/
            error = is_valid_register(token);
            RETURN_IF_ERROR(error);

            param = get_number(token, &pos); 
            param_array[param_needed - 1 - param_count] = itob(param, R_SIZE, 0); /*convert reg num to bit code*/

            token = strtok(NULL, "\n\t\r ");
        }
        if (token == NULL)
        {
            return MISSING_PARAM;
        }
        if ((error = is_label(token, NO_COLON))) /*check last parameter, label*/
        {
            return error;
        } else {
            param_count++;
            token = strtok(NULL, "\n\t\r ");
        }
    } else { /*other I instructions: checks reg, num, reg */ 
        for (param_count = 0; (param_count < param_needed) && (token != NULL); param_count++)
        {
            if (param_count % BINARY != 0) /*read num between regs, set pos accordingly*/
            {
                pos = 0;
                error = is_valid_num(token);
            } else {
                pos = 1; /*pos 0 = $*/
                error = is_valid_register(token);            
            }
            RETURN_IF_ERROR(error);

            param = get_number(token, &pos); 
            param_array[param_needed - 1 - param_count] = itob(param, R_SIZE, 0); /*convert reg/num to bit code*/

            token = strtok(NULL, "\n\t\r ");
        }
    }
    if (param_count < param_needed)
        return MISSING_PARAM;
    if (token != NULL)
        return EXTRANEOUS_OPERAND;

    return OK;
}            
/*
 * Function:  analyse_ins_j 
 * --------------------
 * analyse instructions J
 * opcode_value: opcode from image table to determine instruction
 * token: current string parameter from line
 * param_array: empty array to fill with valid parameters to add to image code
 * returns: returns 0 if all numbers are valid, error number otherwise
 */
enum ERROR_NUM analyse_ins_j(unsigned long opcode_value, char * token, unsigned long param_array[]) {
    enum ERROR_NUM error = OK;

    if (OPC_JUMP(opcode_value) || OPC_CALL_OR_LA(opcode_value))
    {
        if (token == NULL)
            return MISSING_PARAM;
        if (OPC_JUMP(opcode_value))
        {   
            if (START_WITH_DOLLAR_SIGN(token[0])) /*check for valid reg*/
            {
                int pos = 1; /*pos 0 = $*/
                error = is_valid_register(token);
                param_array[0] = itob(get_number(token, &pos), R_SIZE, 0);
            } else {
                error = is_label(token, NO_COLON);
                param_array[0] = itob(MAX_REG + 1, R_SIZE, 0); /*set max_reg + 1 to set reg encoding to 0*/
            }
        } else {
            error = is_label(token, NO_COLON);
        }

        if (error != OK) /*label / register invalid*/
        {
            error = INVALID_PARAM;
        }
        token = strtok(NULL, "\n\t\r ");
    } /*else: cmd is stop - make sure no extraneous operands */

    /*return error if occured & check for extraneous operands*/
    RETURN_IF_ERROR(error);
    if (token != NULL)
        return EXTRANEOUS_OPERAND;

    return OK;
}

/*
 * Function:  string_analysis 
 * --------------------
 * validates string of asciz directive and calculate needed memory
 * this_line: a token extracted from string numbers seperated by commas
 * data_size: stores calculated data allocation needed for string in data memory
 * returns: returns 0 if all numbers are valid, error number otherwise
 */
enum ERROR_NUM string_analysis(char this_line[], int * data_size) {
    int quote_start = 0, quote_end = 0;
    int len = strlen(this_line);
    int i = 0;

    quote_start = get_quote(0, len, this_line); /*first quote appearance */
    quote_end = get_quote(len - 1, strlen(ASCIZ), this_line); /*last quote appearance */

    if (quote_end < quote_start || quote_end == quote_start || !(quote_end && quote_start)) /*start = end or no quots found */
        return INVALID_STRING_QUOT;

    /*get to the char after .asciz - find preiod and add asciz length*/
    while (this_line[i] != '.')
        i++;
    i += strlen(ASCIZ);

    while (isspace(this_line[++i])); /*get first non space char after .asciz */

    if (i != quote_start) /*check no chars before string start*/
        return INVALID_PARAM;

    /*get first char after quote end, check no chars after string end*/
    i = quote_end + 1;
    while ((isspace(this_line[i])) && (i++ < len)); 
    if (i != len) /*last char is not quote */
        return EXTRANEOUS_OPERAND;

    /*copy string between quotes to asci_str*/
    for (i = quote_start + 1; i < quote_end; i++) /*check for non printable char*/
    {
        if (!isprint(this_line[i])) 
            return INVALID_STRING_CHARS;
    }
    *data_size = quote_end - quote_start; /*add byte size to data memory*/
    return OK;
} /* end of string analysis */

/* check if label is saved key word
   given len value pre-calculated to remove colon if exist
   return error num if occured, OK (0) otherwise */
enum ERROR_NUM check_key_word(const char * str, int len) {
    char temp_dir[MAX_LABEL_LENGTH + 1] = {0}; /* compare directive key words, +1 for '.' */
    char temp_ins[MAX_LABEL_LENGTH] = {0}; /* compare instruction key words */

    /*add . to temp_dir & remove ':' if exist for key word compare*/
    temp_dir[0] = '.';
    strncat(temp_dir, str, len);
    strncpy(temp_ins, str, len);

    if (IS_DIRECTIVE(temp_dir) || IS_INSTRUCTION(temp_ins))
        return SYMBOL_KEY_WORD;
    return OK;
}

/*
 * Function:  add_extern_symbol 
 * --------------------
 * Adds extern symbol to symbol table
 * symbol_name: string symbol name
 * existing_symbol: symbol - if symbol exists in table. null otherwise
 * sym_head: symbol table head
 * curr_address: addresses table
 * returns: returns OK if there were no errors, error number otherwise
 */
enum ERROR_NUM add_extern_symbol(char * symbol_name, symbol * existing_symbol, symbol ** sym_head, address ** curr_address) {
    enum ERROR_NUM error;
    if (existing_symbol != NULL) /*existing symbol*/
    {
        if  (existing_symbol->attrib[A_EXTERN] == 0) /*symbol already defined in the program, not as extern */
            return SYMBOL_AMABIGUOUS;
        else /*exists as extern*/
            return OK;
    }
    if (!(error = is_label(symbol_name, NO_COLON))) /*check symbol is valid label*/ 
    {
        add_symbol(sym_head, symbol_name, 0); /*extern address is 0*/
        set_symbol(sym_head, symbol_name, A_EXTERN, SET_ATTRIBUTE);
    } else { /* invalid symbol name */
        return error;
    }
    return OK;
} /* end of add_extern_symbol*/

/* Function: update_code_parameters
 * -------------
 * update code_image with given parameter array
 * param_array: array of parameters to add to image in fixed locations
 * code_value: bit code to update with param values
 * opcode_value: insturction opcode determines how to add parameters to the code
*/
void update_code_parameters(unsigned long param_array[], unsigned long * code_value, unsigned long opcode_value) {
    int i = 0, j = 0, position = 0; /*position in bit code to add parameter*/
    int size = THREE; /*default 3 parameters to add*/

    if (OPC_R(opcode_value)) /* add param: rs, rt, rd*/
    {
        if (opcode_value == 1) 
            size = TWO;

        for (i = 0; i < size; i++, j++)
        {
            if ((opcode_value == 1) && (i == TWO - 1)) /*skip rt on second parameter*/
                j++;

            /*add each param to position from left to right*/
            position = (OPCODE_LOCATION - (R_SIZE * (1 + j))); 
            add_code_parameter(param_array[size -1 -i], &(*code_value), position, R_SIZE);
        }
    }
    if (OPC_I(opcode_value)) /*I instruction*/
    { 
        /*add first parameter to rs*/
        position = (OPCODE_LOCATION - ( R_SIZE * (1 + i)));
        add_code_parameter(param_array[size -1 -i], &(*code_value), position, R_SIZE);

        i++; 
        position = (OPCODE_LOCATION - ( R_SIZE * (1 + i)));

        if (!IS_BRANCHED_CONDITION(opcode_value))/* add num param to immed*/
        {
            add_code_parameter(param_array[size -1 -i], &(*code_value), 0, SIZE_OF_IMMED_I);
            i++;        
        } /*else - add symbol address in 2nd stage*/
        /* add second/third param to rt*/
        add_code_parameter(param_array[size -1 -i], &(*code_value), position, R_SIZE);
    }
    if (OPC_J(opcode_value)) /*J instruction*/
    { 
        if (OPC_JUMP(opcode_value))
        {
            if (btoi(param_array[0], 0, ADDRESS_LENGTH) <= MAX_REG) /*jmp to address in register*/
            {
                position = OPCODE_LOCATION - 1; /*update reg field*/
                add_code_parameter(1, &(*code_value), position, 1);
                position = 0; /*update address field*/
                add_code_parameter(param_array[0], &(*code_value), position, ADDRESS_LENGTH - BITS_IN_BYTE);
            }
            /* else - add symbol address in 2nd stage*/
        }
        /*instructions: la & call address parameter to be determined in 2nd stage. stop address is 0*/
    }
} /*end of update_code_parameters */

/* Function:  is_comment_or_empty_line 
 * --------------------
 * check if given string is an empty line or a comment:
 * str: given line from file
 * returns: returns 1 if empty or comment, 0 otherwise
 */
int is_comment_or_empty_line(const char * str) {
    int i = 0;

    if (str[i] == ';') /* check if comment line */
        return 1; 

    while (isspace(str[i++]) && str[i] != '\0'); /* check if empty line */
    if (str[i] == '\0')
        return 1;

    return 0; 
} /*end of is_comment_or_empty_line */

/*
 * Function:  is_label 
 * --------------------
 * check if given string is a label
 * starts with a letter, contain only letters and digits
 * with or without colon depending on flag
 * must not be a key word
 * str: given string to examine
 * check_colon: flag to check ending colon or not
 * returns: returns 0 (OK) if it is a label, error number otherwise
 */
enum ERROR_NUM is_label(const char * str, enum LABEL_COLON check_colon) {
    int i = 0, len = strlen(str);
    enum ERROR_NUM error = 0;

    /*check ':' if needed*/
    if (check_colon == YES_COLON)
    {
        len--; /*get colon position*/
        if (str[len] != ':')
            return COMMAND_UNRECOGNIZED;
    }
    error = check_key_word(str, len);
    RETURN_IF_ERROR(error);
    
    if (!isalpha(str[i++])) /* check label initial */
        return SYMBOL_INVALID_INITIAL; 

    while (str[i] != '\0') /*check rest of label syntax */
    {
        if (!isalnum(str[i])) /*found non alpha / num char*/
        {
            /*check if colon at end of label, otherwise invalid char*/
            if ( !(check_colon == YES_COLON) || !(str[i] == ':' && str[i + 1] == '\0') ) 
            {
                return SYMBOL_INVALID;
            }
        }
        if (i > MAX_LABEL_LENGTH - 1) /*label exceeds allowed length*/
        {
            return LABEL_INVALID_LENGTH;
        }
        i++;
    }
    return OK; 
} /* end of is_label */

/*get position of quote from start / end of asciz directive, return -1 if not found */
int get_quote(int i, int len, char this_line[]) {
    if (i == 0) 
    { /* get first quote */
        while (this_line[i] != '"' && i++ < len);
        if (this_line[i] == '"')
            return i;
    } 
    else 
    { /* get last quote */
        while (this_line[i] != '"' && i-- > strlen(ASCIZ));
        if (this_line[i] == '"')
            return i;
    }
    return -1;
}

/*
 * Function:  is_valid_num 
 * --------------------
 * validates string number given as parameter
 * doesnt check if null or within data range
 * token: a token extracted from string commands line
 * returns: returns 0 if number is valid, error number otherwise
 */
enum ERROR_NUM is_valid_num(char * token) {
    int pos = 0;
    check_sign(token, &pos);

    if (!all_digits(token, pos))
    {
        return INVALID_PARAM;
    }
    return OK;
}

/*
 * Function:  is_valid_register 
 * --------------------
 * validates string register given as parameter
 * doesnt check if null
 * token: a token extracted from string commands line
 * returns: returns 0 if register is valid, error number otherwise
 */
enum ERROR_NUM is_valid_register(char * token) {
    long param;
    int pos = 0;

    if (START_WITH_DOLLAR_SIGN(token[pos])) 
    {
        pos++; 
        if (!all_digits(token, pos))
        {
            return INVALID_PARAM;
        }
        param = get_number (token, &pos);
        if (param > MAX_REG || param < MIN_REG) { /*check register in range*/
                return REGISTRY_OUT_OF_RANGE;
        }
    } else { /*reg without $ first*/
        return INVALID_PARAM;
    }
    return OK;
}

/*
 * Function:  get_number 
 * --------------------
 * convert string number to long and return it
 * token: given string digits to convert
 * returns: returns long number if token consist of digits, INT_MAX else
 */
long get_number(char * token, int * pos) {
    long num = 0;
    int sign = check_sign(token, pos);
    int i;

    for(i = *pos; i < strlen(token); i++) /*check digits and calculate num*/
    {
        num *= DECIMAL;
        num += (token[(*pos)++] - '0'); /*convert char to num */
    }
    return (num * sign);
}

/*return 1 if num is out of range in terms of data_size, 0 otherwise*/
int is_out_of_range(long num, int data_size){
    if (num > MAX_NUM(data_size) || num < MIN_NUM(data_size)) 
        return 1;
    return 0;
}

/*return 1 if positive, -1 if negative and move pos +1 if sign exists */
int check_sign(char * token, int * pos) {
    if (token[(*pos)] == '-')
    {
        (*pos)++;
        return -1;
    }
    if (token[(*pos)] == '+')
    {
        (*pos)++;
    }
    return 1;
}

/*check string starting from pos consists of digits only
  return 0 if found non digit char, 1 if valid*/
int all_digits(char * str, int pos) {
    int i;
    for(i = pos; i < strlen(str); i++) /*check digits and calculate num*/
    {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}

/*
 * Function:  clear_and_examine_commas 
 * --------------------
 * convert string line with commas to a series of string with space between tham
 * this_line: given line to convert
 * cmd_type: determines if line is headed with label or not
 * returns: return OK if valid, error num otherwise
 */
enum ERROR_NUM clear_and_examine_commas(char this_line[], enum COMMAND_TYPE cmd_type) {
    enum ERROR_NUM error;
    char temp_line[MAX_LINE_LENGTH] = {0}, label_name[MAX_LABEL_LENGTH] = {0}, command_name[MAX_LABEL_LENGTH] = {0};

    if (cmd_type == LABEL_CMD) /*get command name regardless of lable*/
    {
        get_label(this_line, label_name);
        strcpy(temp_line, this_line + (strlen(label_name) + 1)); /*temp line copy line without label*/
        get_command(this_line, command_name, LABEL_CMD);
    } else {
        strcpy(temp_line, this_line);
        get_command(this_line, command_name, !LABEL_CMD);
    }
    /* check & clear commas */
    if (!no_commas_in_command(command_name)) /*check if command require commas or not*/
    {
        if (!(error = check_commas(temp_line))) /*check for error with commas*/
            replace_commas_to_space(this_line);
    } else {
        error = check_no_commas(temp_line); /*check for error with no commas*/
    }
    return error;
}

/* validate no commas in command, return OK if valid, error num otherwise*/
enum ERROR_NUM check_no_commas(char this_line[]) {
    int curr_pos = 0, len = 0;
    char * token;
    token = strtok(this_line, "\n\t\r ");

    if (IS_ASCIZ(token)) /*asciz is validated in string analysis function, comma may appear within quotes*/
        return OK;

    while (token != NULL)
    {
        len = strlen(token);
        while (curr_pos < len)
        {
            if (token[curr_pos] == ',')
                return MISPLACED_COMMA;
            curr_pos++;
        }   
        token = strtok(NULL, "\n\t\r ");
        curr_pos = 0;
    }
    return OK;
}

/* validate invalid commas in line. return 0 if valid, error num otherwise*/
enum ERROR_NUM check_commas(char this_line[]) {
    int flag_comma = 1;
    int curr_pos = 0, len = 0;
    char * token;

    token = strtok(this_line, "\n\t\r ");
    token = strtok(NULL, "\n\t\r ");  

    if (token == NULL) /*command with no additional parameters*/
        return OK;

    if (token[curr_pos] == ',' )
        return MISPLACED_COMMA_START;
    
    /*use flag to make sure only one comma between parameters*/
    while (token != NULL) {
        if (flag_comma == 0 && token[curr_pos] != ',')
            return MISSING_COMMA;
        len = strlen(token);
        while (curr_pos < len) /*check for string token with more than 1 param & comma with no spaces*/
        {
            if (token[curr_pos] == ',')
            {
                if (flag_comma)
                    return CONSECUTIVE_COMMA;
                flag_comma = 1;
            } else { /*char is not comma */
                flag_comma = 0;
            }
            curr_pos++;
        } /*end inner while*/
        token = strtok(NULL, "\n\t\r ");
        curr_pos = 0;
    } /*end outer while*/

    if (flag_comma) /*last char is comma */
        return MISPLACED_COMMA_END;
	return OK;
}

/* replace commas with space */
void replace_commas_to_space(char this_line[]) {
    int curr_pos = 0;
    int len = strlen(this_line);

    while (curr_pos < len) {
        if (this_line[curr_pos] == ',')
        {
            this_line[curr_pos] = ' ';
        }
        curr_pos++;
    }
}

/*return 0 if command needs commas, 1 otherwise*/
int no_commas_in_command(char * command_name) {
    if (IS_ENTRY_OR_EXTERN(command_name) || IS_ASCIZ(command_name) || IS_J(command_name))
        return 1;
    return 0;
}

/*save label string from a line starting with label, without ':' */
void get_label(char this_line[], char label_name[]) {
    char temp_line[MAX_LINE_LENGTH];
    char * token;

    memcpy(temp_line, this_line, MAX_LINE_LENGTH);
    token = strtok(temp_line, "\n\t\r "); 
    token[strlen(token) - 1] = '\0'; /*remove ':' from label */

    memcpy(label_name, token, MAX_LABEL_LENGTH);
}

/*save command string from line, skip label using flag*/
void get_command(char this_line[], char cmd_name[], enum COMMAND_TYPE flag_with_label) {
    char temp_line[MAX_LINE_LENGTH];
    char * token;

    memcpy(temp_line, this_line, MAX_LINE_LENGTH);
    token = strtok(temp_line, "\n\t\r ");
    if (flag_with_label == LABEL_CMD) /*first token is label, get next token*/
        token = strtok(NULL, "\n\t\r ");

    if (token != NULL)
        memcpy(cmd_name, token, MAX_LABEL_LENGTH);
}

/* return location of the last printable char for line length validation*/
int last_printable_char(char original_line[]) {
    int len = strlen(original_line);

    while (!isprint(original_line[len - 1]))
        len--;
    
    return len;
}