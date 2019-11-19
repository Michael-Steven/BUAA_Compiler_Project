#include <stdio.h>
#include <stdlib.h>
#include "word.h"
#include "parser.h"
#include "file.h"
#include "middle_code.h"
#include "mips.h"

extern char c;

int main() {
    //system("rm parser_out.txt middle_code.txt mips.txt error.txt");
    freopen("testfile.txt", "r", stdin);
#ifdef PARSER_OUT
    parser_out = fopen("parser_out.txt", "w");
#endif
#ifdef MIDDLE_CODE_OUT
    middle_code_out = fopen("middle_code.txt", "w");
#endif
#ifdef MIPS_CODE_OUT
    mips_code_out = fopen("mips.txt", "w");
#endif
#ifdef ERROR_OUT
    error_out = fopen("error.txt", "w");
#endif
    c = getchar();
    getsym();
    parse();
    middle_code_print();
    generate_mips();
//    middle_code_print();
    fclose(stdin);
#ifdef PARSER_OUT
    fclose(parser_out);
#endif
#ifdef MIDDLE_CODE_OUT
    fclose(middle_code_out);
#endif
#ifdef MIPS_CODE_OUT
    fclose(mips_code_out);
#endif
#ifdef ERROR_OUT
    fclose(error_out);
#endif
    return 0;
}
