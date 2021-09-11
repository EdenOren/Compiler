#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "error_handler.h"
#include "data_structure.h"
#include "conversions.h"

#define FILE_EXTENSION ".as"
#define MAX_FILE_NAME 100
#define MIN_ARGUMENTS 2 /*needs at least 1 parameter file name on execution*/

/* check file name at lease length of file ext & file ext is valid */
#define VALID_FILE_NAME(str) ( (strlen(str) >= strlen(FILE_EXTENSION)) && \
                             (!(strcmp(str + (strlen(str) - strlen(FILE_EXTENSION)), FILE_EXTENSION))) ) 

int argument_exist(const int argc);
FILE * get_file(const char * file_n);

#endif /* FILE_HANDLER_H */
