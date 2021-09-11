#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "file_handler.h"
#include "data_structure.h"
#include "create_output.h"

void close_and_clear
(
    FILE * file_p,  
    symbol ** sym_head, 
    address ** curr_address, 
    code_t * i_image
);
extern int stage_one
(
    FILE *file_p, 
    const char * file_n, 
    symbol ** sym_head,
    instruction * inst_head, 
    address ** curr_address,  
    code_t * inst_image, 
    code_t * data_image
);
extern int stage_two
(
    FILE *file_p, 
    const char * file_n, 
    symbol ** sym_head, 
    code_t * code_image
);
#endif /* ASSEMBLER_H */
