#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "middle_code.h"
#include "file.h"

Middle_code m_list[10000];
int m_list_len = 0;
int main_location = 0;

//根据传入的类型，参数个数和参数添加中间代码列表
Middle_code middle_code_create(int type, int kind, int num, ...) {
    Middle_code ret;
    int i;
    va_list ap;
    va_start(ap, num);
    ret.type = type;
    ret.c_len = num;
    ret.kind = kind;
    for (i = 0; i < num; i++) {
        strcpy(ret.code[i], va_arg(ap,
                char *));
    }
    va_end(ap);
    return ret;
}

//输出中间代码
void middle_code_print() {
#ifdef MIDDLE_CODE_OUT
    int i, j;
    for (i = 0; i < m_list_len; i++) {
        for (j = 0; j < m_list[i].c_len - 1; j++) {
            fprintf(middle_code_out, "%s ", m_list[i].code[j]);
        }
        fprintf(middle_code_out, "%s", m_list[i].code[m_list[i].c_len - 1]);
        fprintf(middle_code_out, "   kind:%d", m_list[i].kind);
        fprintf(middle_code_out, "\n");
    }
#endif
}
