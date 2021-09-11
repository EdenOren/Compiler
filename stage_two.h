#ifndef STAGE_TWO_H
#define STAGE_TWO_H

/*include headers*/

#include "data_structure.h"
#include "error_handler.h"
#include "stage_one_func.h"
#include "conversions.h"

/*functions*/ 

int stage_two(FILE *file_p, const char * file_n, symbol ** sym_head, code_t * code_image);
enum ERROR_NUM final_analysis(char original_line[], int * line_num, symbol ** sym_head, code_t * code_image, int * address_count);
enum ERROR_NUM final_instruction_analysis(char * token, symbol ** sym_head, code_t * code_image, int * address_count);
enum ERROR_NUM add_entry_symbol(char * symbol_name, symbol ** sym_head);
enum COMMAND_TYPE get_cmd_type(char original_line[]);
void add_extern_address(symbol ** sym_head, char * token, int symbol_address, int address_count);
#endif /* STAGE_TWO_H */

