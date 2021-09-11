#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include <string.h>
#include <stdlib.h>
#include "error_handler.h"

#define BITS_IN_BYTE 8
#define ADDRESS_LENGTH 32 /*each address has 32 bits */
#define IC_MIN_ADDRESS 100 /* instructions count min address*/
#define IC_ADDRESS_SIZE 4 /*each instruction size is 4 bytes */
#define MAX_MEMORY_SIZE 33554432 /*size of memory: 2^25*/
#define ATTRIBUTE_NUM 4 /*number of possible attributes for a symbol */

enum COUNTER {IS_IC, IS_DC}; /*instruction / data counter */
enum SYMBOL_ATTRIBUTE {A_INST, A_DATA, A_ENTRY, A_EXTERN}; /*symbol attributes */
enum SYMBOL_SET_TYPE {SET_ADDRESS, SET_ATTRIBUTE};  /*value to set at function */

/*instructions table definitions */

#define INSTRUCTIONS_TABLE "add", "sub", "and", "or", "nor", "move","mvhi", "mvlo", "addi", "subi", \
                           "andi", "ori","nori", "bne", "beq", "blt", "bgt", "lb","sb", "lw", "sw", \
                           "lh", "sh", "jmp", "la", "call", "stop"
#define OPCODE_LOCATION 26 /*location of opcode in address*/
#define FUNCT_LOCATION 6 /*location of funct in address*/
#define R_OP_0_COUNT 5 /*total R commands with opcode 0 */
#define R_OP_1_COUNT 3 /*total R commands with opcode 1 */
#define R_COUNT (R_OP_0_COUNT + R_OP_1_COUNT) /*total R commands */
#define I_COUNT 15 /*total I commands */
#define J_COUNT 4 /*total J commands */
#define IS_NOT_STOP(str) (strcmp(str, "stop"))

enum INSTRUCTIONS_OPCODE {OPC_I = 10, OPC_J = 30, OPC_STOP = 63}; /*instruction table opcodes*/

/* data struct types */
typedef struct data_array /*data size for each directive*/
{
   int data_size;
   struct data_array * next;
} data_array;

typedef struct address {
   int IC; /*insturctions count */
   int DC; /*data count */
   data_array * data_arr;
} address;

typedef struct symbol {
   char label[MAX_LABEL_LENGTH]; /*symbol name*/
   unsigned long address; /*address of symbol - decimal*/
   int attrib[ATTRIBUTE_NUM]; /*attribute: code, data, entry, external*/
   struct symbol * next;
} symbol;

typedef struct instruction {
   char label[MAX_LABEL_LENGTH]; /*symbol name*/
   unsigned long value; /*machine code - 32 bits - binary*/
   struct instruction * next;
} instruction;

typedef struct code_table {
   unsigned char value; /*machine code - 8 bits - binary*/
} code_t;

/*symbol table functions */

void add_symbol(symbol** head, const char * token, unsigned long insert_address);
void set_symbol(symbol** head, const char * token, unsigned long value, enum SYMBOL_SET_TYPE type);
symbol * get_symbol(symbol** head, const char * token);
void delete_symbol_table(symbol** head);
void update_symbol_addresses(symbol** head, const int icf);

/*instructions table functions */

instruction * create_instructions_table ();
instruction * get_instruction (instruction * head, char * str);
void delete_instructions_table (instruction * head);

/*address functions */

void init_addresses(address ** curr_address);
void set_address(address ** curr_address, const int add, const int type);
unsigned long get_address(address ** curr_address, enum COUNTER type);

/*memory table functions */

void init_code_table(code_t ** code_table);
void create_code_image(code_t * code_image, code_t * d_image, const int dcf, const int icf);
void add_to_image(code_t * code_table, unsigned long code_address, unsigned long encoding, int data_size);
void get_from_image(code_t * code_table, unsigned long code_address, unsigned long * encoding, int data_size);


/*data size table functions */

void add_data_size(data_array ** head, int add);
void delete_data_size_table(data_array** head);

#endif /* DATA_STRUCT_H */
