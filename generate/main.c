#include <stdio.h>
#include "word.h"
#include "parser.h"
#include "file.h"

int main() {
    freopen("testfile.txt", "r", stdin);
#ifdef PARSER_OUT
    parser_out = fopen("parser_out.txt", "w");
#endif
#ifdef MIDDLE_CODE_OUT
    middle_code = fopen("middle_code.txt", "w");
#endif
#ifdef MIPS_CODE_OUT
    mips_code = fopen("mips.txt", "w");
#endif
#ifdef ERROR_OUT
    error = fopen("error.txt", "w");
#endif
    c = getchar();
    getsym();
    parse();
    fclose(stdin);
#ifdef PARSER_OUT
    fclose(parser_out);
#endif
#ifdef MIDDLE_CODE_OUT
    fclose(middle_code);
#endif
#ifdef MIPS_CODE_OUT
    fclose(mips_code);
#endif
#ifdef ERROR_OUT
    fclose(error);
#endif
    return 0;
}
