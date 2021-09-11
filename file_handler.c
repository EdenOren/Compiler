#include "file_handler.h"

/* checks if program was given at least one parameter file for compiling */
int argument_exist(const int argc) {
    if (argc < MIN_ARGUMENTS)
    {
        print_fundamental_error(NO_FILE_PARAMETER, NULL);
        return 1;
    }
    return 0;
}
/* checks if file parameter has valid name
   and return pointer to file (if successfully opened).
   prints error and return NULL otherwise  */
FILE * get_file(const char * file_n) {
    FILE * file_p = NULL;
    if (VALID_FILE_NAME(file_n))
    {
        file_p = fopen(file_n, "r");
        if (file_p == NULL)
        {
            print_fundamental_error(FILE_OPENING_ERROR, file_n);
            return NULL;
        }
    } else {
        print_fundamental_error(FILE_INVALID_NAME, file_n);
        return NULL;
    }
    return file_p;
} /*end of get_files */
