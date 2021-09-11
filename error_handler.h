#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <stdio.h>

#define MAX_LINE_LENGTH 81 /* max 80 chars for line, +1 for terminator */
#define MAX_LABEL_LENGTH 32 /*max 31 chars for label, +1 for terminator */

/*error values */
enum ERROR_NUM{OK = 0, NO_AVAILABLE_MEMORY, MEMORY_OVERFLOW, LINE_INVALID_LENGTH, LABEL_INVALID_LENGTH,     \
      SYMBOL_INVALID_INITIAL, SYMBOL_INVALID, COMMAND_UNRECOGNIZED, EXTRANEOUS_OPERAND, MISPLACED_COMMA,    \
      MISPLACED_COMMA_START, MISPLACED_COMMA_END, MISSING_PARAM, CONSECUTIVE_COMMA, INVALID_PARAM,          \
      OUT_OF_MEMORY, INVALID_STRING_QUOT, INVALID_STRING_CHARS, SYMBOL_KEY_WORD, DUPLICATE_SYMBOL,          \
      MISSING_SYMBOL, SYMBOL_AMABIGUOUS, REGISTRY_OUT_OF_RANGE, MISSING_COMMA, SYMBOL_IS_EXTERN,            \
      IMMED_OUT_OF_RANGE, FILE_READING_ERROR, NO_FILE_PARAMETER, FILE_OPENING_ERROR, FILE_INVALID_NAME};    \
      
void print_error(const enum ERROR_NUM error, const char * file_n, const int line_num, const char * token);
void print_fundamental_error(const enum ERROR_NUM error, const char * file_n);

#endif /* ERROR_HANDLER_H */