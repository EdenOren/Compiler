#include "error_handler.h"
/*
 * Function:  print_error 
 * --------------------
 * prints errors to stdout
 * error: error to print
 * file_n: current file name
 * line_num: error line number
 * token: the string of the error if available
 */
void print_error(const enum ERROR_NUM error, const char * file_n, const int line_num, const char * token) {

    printf("%s, Line: %-3d - Error: ", file_n, line_num);
    switch (error) {
        /*1st stage errors*/
        case LABEL_INVALID_LENGTH:      printf("Label exceeds allowed length of %d", MAX_LABEL_LENGTH - 1); break;
        case LINE_INVALID_LENGTH:       printf("Line exceeds allowed length of %d", MAX_LINE_LENGTH - 1); break;
        case SYMBOL_INVALID_INITIAL:    printf("Invalid symbol initial"); break;
        case SYMBOL_INVALID:            printf("Invalid symbol syntax"); break;
        case SYMBOL_KEY_WORD:           printf("Symbol is a saved key word"); break;
        case COMMAND_UNRECOGNIZED:      printf("Unrecognized command"); break;
        case EXTRANEOUS_OPERAND:        printf("Extraneous operand"); break;
        case MISPLACED_COMMA:           printf("Misplaced comma"); break;
        case MISPLACED_COMMA_START:     printf("Misplaced comma before parameters"); break;
        case MISPLACED_COMMA_END:       printf("Misplaced comma after parameters"); break;
        case CONSECUTIVE_COMMA:         printf("Consecutive comma"); break;
        case MISSING_COMMA:             printf("Missing comma between parameters"); break;
        case MISSING_PARAM:             printf("Missing parameter"); break;
        case INVALID_PARAM:             printf("Invalid parameter"); break;
        case REGISTRY_OUT_OF_RANGE:     printf("Registery is out of range 0-31"); break;
        case INVALID_STRING_QUOT:       printf("Invalid string: missing quotes"); break;
        case INVALID_STRING_CHARS:      printf("Invalid string: unprintable chars"); break;        
        case DUPLICATE_SYMBOL:          printf("Symbol already exists, duplicate definition"); break;
        /*2nd stage errors*/
        case IMMED_OUT_OF_RANGE:        printf("Immed range to label out of range"); break;
        case SYMBOL_AMABIGUOUS:         printf("Symbol already exists, ambiguous definition"); break;
        case SYMBOL_IS_EXTERN:          printf("Symbol in conditional branch is extern"); break;
        case MISSING_SYMBOL:            printf("Missing or undefined symbol"); break;
        default:                        printf("Unknown error"); break;
    } /*end of switch */

    /*print token, exculding length related errors or missing token value*/
    if (token == NULL || error == LABEL_INVALID_LENGTH || error == LINE_INVALID_LENGTH)
        printf(" \n");
    else    
        printf(" <%s>\n", token);
} 

/*
 * Function:  print_fundamental_error 
 * --------------------
 * prints critical errors to stdout
 * error: error to print
 * file_n: file name if given
 */
void print_fundamental_error(const enum ERROR_NUM error, const char * file_n) {
    printf("Error: ");
    switch (error) {
        case MEMORY_OVERFLOW:           printf("Memory overflow"); break;
        case NO_AVAILABLE_MEMORY:       printf("No available memory to allocate"); break;
        case FILE_OPENING_ERROR:        printf("In opening the file"); break;
        case FILE_READING_ERROR:        printf("While trying to re-read a file"); break;
        case NO_FILE_PARAMETER:         printf("Missing input files"); break;
        case FILE_INVALID_NAME:         printf("File extension is not valid"); break;
        default:                        printf("Unknown error"); break;
    }
    if (file_n == NULL)
        printf("\n");
    else    
        printf(" <%s>\n", file_n);
} 