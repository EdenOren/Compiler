#ifndef CREATE_OUTPUT_H
#define CREATE_OUTPUT_H

#include "file_handler.h"


#define OBJ_EXT ".ob"
#define ENTRY_EXT ".ent"
#define EXTERN_EXT ".ext"
#define DATA_SIZE_END -1

enum FILE_OUTPUT_TYPE{OBJ_OUT, EXT_OUT, ENT_OUT};

void create_output(symbol ** sym_head, char * file_n, code_t * code_image, int ICF, int DCF, data_array * data_size_array);
FILE * get_output_file_name(char * fname, enum FILE_OUTPUT_TYPE type);

void object_output(char * fname, code_t * code_image, int ICF, int DCF, data_array * data_size_array);
void extern_entry_output(char * fname, symbol ** table, enum SYMBOL_ATTRIBUTE type);

void print_new_row(FILE * file_p, int current_index, int print_index, int * last_print_index, int dsize);
int get_dsize(int current_index, int print_index, int ICF, data_array ** data_size_array);

#endif /* CREATE_OUTPUT_H */