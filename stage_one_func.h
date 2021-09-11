#ifndef STAGE_ONE_FUNC_H
#define STAGE_ONE_FUNC_H

#include "stage_one.h"

/*enums */

enum DATA_TYPE        {DB = 1, DH = 2, DW = 4}; /*data image bytes*/
enum LABEL_COLON      {NO_COLON, YES_COLON}; /* check for colon or not */
enum PARAMETER_NUMBER {TWO = 2, THREE = 3};

/*definitions */

#define MAX_NUM(bytes) ((1 << (BITS_IN_BYTE * bytes - 1)) - 1) /* max possible number value of signed int by amount of bits */
#define MIN_NUM(bytes) ((-1) * MAX_NUM(bytes))
#define RETURN_IF_ERROR(error) if(error) { return error; }

/*instructions related defs*/

#define IS_BRANCHED_CONDITION(opcode) ((opcode >= 15 && opcode <= 18))
#define OPC_JUMP(opcode) ((opcode == 30))
#define OPC_CALL_OR_LA(opcode) ((opcode >= 31 && opcode <= 32))
#define OPC_R(opcode) ((opcode >= 0 && opcode <= 1))
#define OPC_I(opcode) ((opcode >= 10 && opcode <= 24))
#define OPC_J(opcode) (OPC_JUMP(opcode) || OPC_CALL_OR_LA(opcode) || (opcode == 63))
#define SIZE_OF_IMMED_I (BITS_IN_BYTE * 2)
#define MAX_REG 31
#define MIN_REG 0
#define START_WITH_DOLLAR_SIGN(char) ((char == '$')) 
#define R_SIZE 5
#define IS_R(str) (!( (strcmp(str, "add")) && (strcmp(str, "sub")) && (strcmp(str, "and")) && (strcmp(str, "or"))      &&\
                  (strcmp(str, "nor")) && (strcmp(str, "move")) && (strcmp(str, "mvhi")) && (strcmp(str, "mvlo")) ))

#define IS_I(str) (!( (strcmp(str, "addi")) && (strcmp(str, "subi")) && (strcmp(str, "andi")) && (strcmp(str, "ori"))  &&\
                  (strcmp(str, "nori")) && (strcmp(str, "bne")) && (strcmp(str, "beq")) && (strcmp(str, "blt"))        &&\
                  (strcmp(str, "bgt")) && (strcmp(str, "lb")) && (strcmp(str, "sb")) && (strcmp(str, "lw"))            &&\
                  (strcmp(str, "sw")) && (strcmp(str, "lh")) && (strcmp(str, "sh")) ))

#define IS_J(str) (!( (strcmp(str, "jmp")) && (strcmp(str, "la")) && (strcmp(str, "call")) && (strcmp(str, "stop")) ))
#define IS_INSTRUCTION(str) (IS_R(str) || IS_I(str) || IS_J(str))

/*directives related defs*/

#define IS_DATA_DIRECTIVE(token) (!strcmp(token, ".db") || !strcmp(token, ".dh") || !strcmp(token, ".dw"))
#define IS_EXTERN(token) (!strcmp(token, ".extern"))
#define IS_ENTRY(token) (!strcmp(token, ".entry"))
#define IS_ENTRY_OR_EXTERN(token) (IS_ENTRY(token) || IS_EXTERN(token))
#define ASCIZ ".asciz"
#define IS_ASCIZ(token) (!strcmp(token, ASCIZ))
#define IS_DIRECTIVE(str) (!( (strcmp(str, ".dh")) && (strcmp(str, ".dw")) && (strcmp(str, ".db")) &&\
                          (strcmp(str, ".asciz")) && (strcmp(str, ".entry")) && (strcmp(str, ".extern")) ))
/*functions*/ 

enum ERROR_NUM analyse_ins_r(unsigned long opcode_value, char * token, unsigned long param_array[]);
enum ERROR_NUM analyse_ins_i(unsigned long opcode_value, char * token, unsigned long param_array[]);
enum ERROR_NUM analyse_ins_j(unsigned long opcode_value, char * token, unsigned long param_array[]);

enum ERROR_NUM add_extern_symbol(char * symbol_name, symbol * existing_symbol, symbol ** sym_head, address ** curr_address);
void update_code_parameters(unsigned long param_array[], unsigned long * code_value, unsigned long opcode_value);
int last_printable_char(char original_line[]);

/*validate numbers and registers functions */

enum ERROR_NUM is_valid_num(char * token);
enum ERROR_NUM is_valid_register(char * token);
int is_out_of_range(long num, int data_size);
long get_number(char * token, int * pos);
int check_sign(char * token, int * pos);
int all_digits(char * str, int pos);

/*string extractions and validation commands */

enum ERROR_NUM is_label(const char * str, enum LABEL_COLON check_colon);
enum ERROR_NUM string_analysis(char this_line[], int * data_size);
int get_quote(int i, int len, char this_line[]);
void get_label(char this_line[], char label_name[]);
void get_command(char this_line[], char cmd_name[], enum COMMAND_TYPE flag_with_label);
enum ERROR_NUM check_key_word(const char * str, int len);

/*comma related functions */

enum ERROR_NUM clear_and_examine_commas(char this_line[], enum COMMAND_TYPE cmd_type);
enum ERROR_NUM check_no_commas(char this_line[]);
enum ERROR_NUM check_commas(char this_line[]);
void replace_commas_to_space(char this_line[]);
int no_commas_in_command(char * command);
int is_comment_or_empty_line(const char * str);

#endif /* STAGE_ONE_FUNC_H */