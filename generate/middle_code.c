#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "middle_code.h"
#include "file.h"
#include "error.h"

Middle_code m_list[10000];
int m_list_len = 0;
int main_location = 0;
Inline_func inline_list[100]; // 内联函数表
int ninline = 0;

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
        fprintf(middle_code_out, "   type:%d", m_list[i].type);
        fprintf(middle_code_out, "\n");
    }
#endif
}

void middle_code_optimize() {
//    clear();
//    inline_optimize();
}

void clear() {
    int i, j, k, flag = 0, delete, n;
    while (1) {
        delete = 0;
        for (i = 0; i < m_list_len; i++) {
            if (m_list[i].type == CALCULATE) {
                flag = 0;
                n = strlen(m_list[i].code[0]);
                for (j = 0; j < n; j++) {
                    if (m_list[i].code[0][j] == '[') {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 1) continue;
                if (equal(m_list[i].code[0], m_list[i].code[2])) continue;
                flag = 0;
                for (j = i + 1; j < m_list_len; j++) {
                    if (m_list[j].type == CALCULATE && equal(m_list[j].code[0], m_list[i].code[0]) &&
                        !equal(m_list[j].code[2], m_list[i].code[0]) &&
                        !equal(m_list[j].code[4], m_list[i].code[0])/* && flag == 0*/) {
                        break;
                    }
                    for (k = 0; k < m_list[j].c_len; k++) {
                        if (equal(m_list[j].code[k], m_list[i].code[0])) {
                            flag = 1;
                            break;
                        }
                    }
                    if (flag == 1) {
                        break;
                    }
                }
                if (flag == 0) {
                    for (j = i; j < m_list_len - 1; j++) {
                        m_list[j] = m_list[j + 1];
                    }
                    delete = 1;
                    m_list_len--;
                    if (i < main_location) {
                        main_location--;
                    }
                }
            }
        }
        if (delete == 0) {
            break;
        }
    }
}

void inline_optimize() {
    int i, j, k, l, head, tail;
    for (i = 0; i < main_location; i++) {
        if (m_list[i].type == DECLARATION_HEADER) {
            inline_list_create(i);
            i++;
            while (m_list[i].type != DECLARATION_HEADER && m_list[i].type != MAIN_FUNC) i++;
            i--;
        }
    }
    for (i = 0; i < m_list_len; i++) {
        if (m_list[i].type == FUNCTION_CALL) {
            for (j = 0; j < ninline; j++) {
                if (equal(m_list[i].code[1], inline_list[j].name)) {
                    int nparam = 0;
                    head = i;
                    tail = i;
                    while (m_list[head - 1].type == FUNCTION_CALL_PARAMETER) head--;
                    while (m_list[tail].type != ASSIGNMENT_STATEMENT) tail++;
                    for (k = head; k <= tail; k++) {
                        if (m_list[k].type == FUNCTION_CALL_PARAMETER) {
                            m_list[k] = middle_code_create(ASSIGNMENT_STATEMENT, m_list[k].kind, 3,
                                                           inline_list[j].param[nparam++], "=", m_list[k].code[1]);
                        }
                        else if (m_list[k].type == FUNCTION_CALL) {
                            inline_code_insert(j, k, &tail);
                            k--;
                        }
                        else if (m_list[k].type == ASSIGNMENT_STATEMENT && equal(m_list[k].code[2], "RET")) {
                            strcpy(m_list[k].code[2], inline_list[j].ret);
                        }
                    }
                }
            }
        }
    }
//    printf("1");
}

int inline_list_create(int location) {
    int i, param_sum = 0;
    for (i = location + 1; m_list[i].type != LABLE; i++) {
        if (m_list[i].type == PARAMETER) param_sum++;
        if (!(m_list[i].type == PARAMETER || m_list[i].type == CALCULATE || m_list[i].type == ASSIGNMENT_STATEMENT ||
              m_list[i].type == RETURN_STATEMENT) || param_sum > 4)
            return 0;
    }
    for (i = location; m_list[i].type != LABLE; i++) {
        if (m_list[i].type == DECLARATION_HEADER) {
            strcpy(inline_list[ninline].name, m_list[i].code[1]);
        }
        else if (m_list[i].type == PARAMETER) {
            strcpy(inline_list[ninline].param[inline_list[ninline].nparam++], m_list[i].code[2]);
        }
        else if (m_list[i].type == RETURN_STATEMENT) {
            strcpy(inline_list[ninline].ret, m_list[i].code[1]);
        }
        else {
            inline_list[ninline].code[inline_list[ninline].ncode++] = m_list[i];
        }
    }
    ninline++;
    return 1;
}

void inline_code_insert(int n, int location, int *end) {
    int i, j, k, flag;
    Middle_code temp;
    for (i = location; i < m_list_len - 1; i++) {
        m_list[i] = m_list[i + 1];
    }
    m_list_len--;
    if (location < main_location) {
        main_location--;
    }
    (*end)--;
    for (i = 0; i < inline_list[n].ncode; i++) {
        temp.type = inline_list[n].code[i].type;
        temp.c_len = 0;
        temp.kind = inline_list[n].code[i].kind;
        for (j = 0; j < inline_list[n].code[i].c_len; j++) {
            strcpy(temp.code[temp.c_len++], inline_list[n].code[i].code[j]);
        }
        for (j = m_list_len; j > location; j--) {
            m_list[j] = m_list[j - 1];
        }
        m_list[location] = temp;
        m_list_len++;
        if (location < main_location) {
            main_location++;
        }
        (*end)++;
        location++;
    }
}