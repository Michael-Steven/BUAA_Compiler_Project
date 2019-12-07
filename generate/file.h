#ifndef GENERATE_FILE_H
#define GENERATE_FILE_H

//#define PARSER_OUT
#define MIDDLE_CODE_OUT
#define MIPS_CODE_OUT
#define ERROR_OUT

#include <stdio.h>

FILE *parser_out;
FILE *middle_code_out;
FILE *mips_code_out;
FILE *error_out;

#endif //GENERATE_FILE_H
