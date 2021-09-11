#ifndef STAGE_ONE_H
#define STAGE_ONE_H

#define BUFF 1

/*include standard library*/

#include <ctype.h>
#include <limits.h>

/*enums */

enum COMMAND_TYPE {NOT_CLASSIFIED, LABEL_CMD, DIRECTIVE_CMD, INSTRUCTION_CMD}; /*command line types */

/*include other headers*/

#include "data_structure.h"
#include "error_handler.h"
#include "stage_one_func.h"
#include "conversions.h"

/*functions*/ 

int stage_one
(
    FILE *file_p, 
    const char * file_n, 
    symbol ** sym_head,
    instruction * inst_head, 
    address ** curr_address,  
    code_t * inst_image, 
    code_t * data_image
);

enum COMMAND_TYPE preliminary_analysis
(
    FILE *file_p,
    const char * file_n,
    char original_line[],
    int line_num,
    int * error_flag
);

enum ERROR_NUM analyse_type
(
    enum COMMAND_TYPE cmd_type, 
    char this_line[], 
    symbol ** sym_head,
    instruction * inst_head, 
    address ** curr_address,
    code_t * inst_image, 
    code_t * data_image
);

enum ERROR_NUM analyse_instruction
(
    char this_line[], 
    address ** curr_address,
    symbol ** sym_head, 
    instruction * inst_head, 
    code_t * inst_image
);

enum ERROR_NUM analyse_directive
(
    char this_line[], 
    address ** curr_address, 
    symbol ** sym_head, 
    code_t * data_image
);

enum ERROR_NUM analyse_label
(
    char this_line[], 
    address ** curr_address, 
    symbol ** sym_head, 
    instruction * inst_head, 
    code_t * inst_image, 
    code_t * data_image
);



#endif /* STAGE_ONE_H */

