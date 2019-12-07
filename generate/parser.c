#include <stdio.h>
#include <string.h>
#include "word.h"
#include "middle_code.h"
#include "parser.h"
#include "error.h"
#include "file.h"

Element sym;    // "getsym()" save a word struct in sym
Element pre_read1, pre_read2;
int var1 = 0, var2 = 0;
int reg_cnt = 0;
int label = 0;
//符号表相关内容
char return_func[100][100] = {};
int return_func_kind[100];
char no_return_func[100][100] = {};
int return_func_index = 0;
int no_return_func_index = 0;
Var_list var_list[1000];
int var_list_index;
const char mark[40][10] = {"IDENFR", "INTCON", "CHARCON", "STRCON", "CONSTTK", "INTTK", "CHARTK",
                           "VOIDTK", "MAINTK", "IFTK", "ELSETK", "DOTK", "WHILETK", "FORTK",
                           "SCANFTK", "PRINTFTK", "RETURNTK", "PLUS", "MINU", "MULT", "DIV",
                           "LSS", "LEQ", "GRE", "GEQ", "EQL", "NEQ", "ASSIGN", "SEMICN",
                           "COMMA", "LPARENT", "RPARENT", "LBRACK", "RBRACK", "LBRACE", "RBRACE"};
extern Middle_code m_list[10000];
extern int m_list_len;
extern int main_location;

int parse() {
    if (program().ret < 0) {
        handle_error("compile error\n");
        return 0;
    }
    return 0;
}

void parser_print(char *string) {
#ifdef PARSER_OUT
    fprintf(parser_out, "%s", string);
#endif
}

Parser program() {
    Parser r;
    check(constant_specification())
    check(variable_specification())
    while (1) {
        if (return_func_definition().ret < 0) {
            if (no_return_func_definition().ret < 0) {
                break;
            }
        }
    }
    check(main_func())
    parser_print("<程序>\n");
    return zero;
}

Parser constant_specification() {
    Parser r;
    if (!equal(sym.content, "const")) {
        return zero;
    }
    while (equal(sym.content, "const")) {
        print_word(sym);
        getsym();
        check(constant_definition())
        if (!equal(sym.content, ";")) {
            handle_error("no ';' behind const definition");
            return negative;
        }
        print_word(sym);
        getsym();
    }
    parser_print("<常量说明>\n");
    return zero;
}

Parser constant_definition() {
    Parser r;
    char identifier[100];
    int l = 1;
    if (equal(sym.content, "int")) {
        while (l == 1) {
            print_word(sym);
            getsym();
            if (!equal(mark[sym.type], "IDENFR")) {
                handle_error("wrong identifier!");
                return negative;
            }
            if ((l = find_var(sym.content)) != -1) {
                sprintf(var_list[l].replace, "$temp_%d", reg_cnt++);
                var_list[l].usage = 1;
                strcpy(identifier, var_list[l].replace);
                var_list[l].replace_kind = 0;
            }
            else {
                strcpy(var_list[var_list_index].name, sym.content);
                var_list[var_list_index].kind = 0;
                var_list[var_list_index++].usage = 0;
                strcpy(identifier, sym.content);
            }
            print_word(sym);
            getsym();
            if (!equal(sym.content, "=")) {
                handle_error("no '=' behind const int, int");
                return negative;
            }
            print_word(sym);
            getsym();
            check(integer())
            m_list[m_list_len++] = middle_code_create(
                    CONSTANT_SPECIFICATION, 0, 5, "const", "int", identifier, "=", r.content[0]);
            l = equal(sym.content, ",");
        }
        parser_print("<常量定义>\n");
        return zero;
    }
    if (equal(sym.content, "char")) {
        while (l == 1) {
            print_word(sym);
            getsym();
            if (!equal(mark[sym.type], "IDENFR")) {
                handle_error("wrong identifier!");
                return negative;
            }
            if ((l = find_var(sym.content)) != -1) {
                sprintf(var_list[l].replace, "$temp_%d", reg_cnt++);
                var_list[l].usage = 1;
                strcpy(identifier, var_list[l].replace);
                var_list[l].replace_kind = 1;
            }
            else {
                strcpy(var_list[var_list_index].name, sym.content);
                var_list[var_list_index].kind = 1;
                var_list[var_list_index++].usage = 0;
                strcpy(identifier, sym.content);
            }
//            strcpy(identifier, sym.content);
            print_word(sym);
            getsym();
            if (!equal(sym.content, "=")) {
                handle_error("no '=' behind const char, char");
                return negative;
            }
            print_word(sym);
            getsym();
            if (!equal(mark[sym.type], "CHARCON")) {
                handle_error("wrong character");
                return negative;
            }
            m_list[m_list_len++] = middle_code_create(
                    CONSTANT_SPECIFICATION, 1, 5, "const", "char", identifier, "=", sym.content);
            print_word(sym);
            getsym();
            l = equal(sym.content, ",");
        }
        parser_print("<常量定义>\n");
        return zero;
    }
    return negative;
}

int find_var(char *name) {
    int i;
    for (i = 0; i < var_list_index; i++) {
        if (equal(var_list[i].name, name)) {
            return i;
        }
    }
    return -1;
}

Parser integer() {
    Parser r, ret = {0, 1};
    if ((equal(sym.content, "+") || equal(sym.content, "-")) && sym.type != 2) {
        strcpy(ret.content[0], sym.content);
        print_word(sym);
        getsym();
    }
    check(unsigned_integer())
    strcat(ret.content[0], r.content[0]);
    parser_print("<整数>\n");
    return ret;
}

Parser unsigned_integer() {
    Parser ret = {0, 1};
    if (!equal(mark[sym.type], "INTCON")) {
        return negative;
    }
    strcpy(ret.content[0], sym.content);
    print_word(sym);
    getsym();
    parser_print("<无符号整数>\n");
    return ret;
}

Parser variable_specification() {
    Parser r;
    int cnt = 0;
    while (1) {
        if (!(equal(sym.content, "int") || equal(sym.content, "char"))) {
            break;
        }
        strcpy(pre_read1.content, sym.content);
        pre_read1.type = sym.type;
        var1 = 1;
        getsym();
        strcpy(pre_read2.content, sym.content);
        pre_read2.type = sym.type;
        var2 = 1;
        getsym();
        if (!equal(sym.content, "[")
            && !equal(sym.content, ",")
            && !equal(sym.content, ";")) {
            break;
        }
        check(variable_definition())
        cnt++;
        if (!equal(sym.content, ";")) {
            handle_error("no ';' behind variable definition");
            return negative;
        }
        print_word(sym);
        getsym();
    }
    if (cnt == 0) {
        return zero;
    }
    parser_print("<变量说明>\n");
    return zero;
}

Parser variable_definition() {
    Parser r;
    char type[10], identifier[100], len[100];
    int is_array = 0, l, kind;
    strcpy(type, pre_read1.content);
    kind = equal(pre_read1.content, "int") ? 0 : 1;
    print_word(pre_read1);
    var1 = 0;
    if (!equal(mark[pre_read2.type], "IDENFR")) {
        handle_error("wrong identifier!");
        return negative;
    }
    if ((l = find_var(pre_read2.content)) != -1) {
        sprintf(var_list[l].replace, "$temp_%d", reg_cnt++);
        var_list[l].usage = 1;
        strcpy(identifier, var_list[l].replace);
        var_list[l].replace_kind = kind;
    }
    else {
        strcpy(var_list[var_list_index].name, pre_read2.content);
        var_list[var_list_index].kind = kind;
        var_list[var_list_index++].usage = 0;
        strcpy(identifier, pre_read2.content);
    }
//    strcpy(identifier, pre_read2.content);
    print_word(pre_read2);
    var2 = 0;
    if (equal(sym.content, "[")) {
        is_array = 1;
        print_word(sym);
        getsym();
        r = unsigned_integer();
        if (equal(r.content[0], "0")) {
            handle_error("number of array is 0");
            return negative;
        }
        strcpy(len, r.content[0]);
        if (!equal(sym.content, "]")) {
            handle_error("no ']' after '['");
            return negative;
        }
        print_word(sym);
        getsym();
    }
    if (is_array == 1) {
        m_list[m_list_len++] = middle_code_create(VARIABLE_SPECIFICATION, kind, 4, "var", type, identifier, len);
    }
    else {
        m_list[m_list_len++] = middle_code_create(VARIABLE_SPECIFICATION, kind, 3, "var", type, identifier);
    }
    is_array = 0;
    while (equal(sym.content, ",")) {
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return negative;
        }
        if ((l = find_var(sym.content)) != -1) {
            sprintf(var_list[l].replace, "$temp_%d", reg_cnt++);
            var_list[l].usage = 1;
            strcpy(identifier, var_list[l].replace);
            var_list[l].replace_kind = kind;
        }
        else {
            strcpy(var_list[var_list_index].name, sym.content);
            var_list[var_list_index].kind = kind;
            var_list[var_list_index++].usage = 0;
            strcpy(identifier, sym.content);
        }
//        strcpy(identifier, sym.content);
        print_word(sym);
        getsym();
        if (equal(sym.content, "[")) {
            is_array = 1;
            print_word(sym);
            getsym();
            r = unsigned_integer();
            if (equal(r.content[0], "0")) {
                handle_error("number of array is 0");
                return negative;
            }
            strcpy(len, r.content[0]);
            if (!equal(sym.content, "]")) {
                handle_error("no ']' after '['");
                return negative;
            }
            print_word(sym);
            getsym();
        }
        if (is_array == 1) {
            m_list[m_list_len++] = middle_code_create(VARIABLE_SPECIFICATION, kind, 4, "var", type, identifier, len);
        }
        else {
            m_list[m_list_len++] = middle_code_create(VARIABLE_SPECIFICATION, kind, 3, "var", type, identifier);
        }
        is_array = 0;
    }
    parser_print("<变量定义>\n");
    return zero;
}

Parser return_func_definition() {
    Parser r;
    int i;
//    roll_back_index = var_list_index;
    check(declaration_header())
    if (!equal(sym.content, "(")) {
        handle_error("no '(' after func header");
        return negative;
    }
    print_word(sym);
    getsym();
    check(parameter_table())
    if (!equal(sym.content, ")")) {
        handle_error("no ')' after '('");
        return negative;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, "{")) {
        handle_error("no '{' after func header");
        return negative;
    }
    print_word(sym);
    getsym();
    check(compound_statement())
    if (!equal(sym.content, "}")) {
        handle_error("no '}' after '{'");
        return negative;
    }
    print_word(sym);
    getsym();
    for (i = 0; i < var_list_index; i++) {
        var_list[i].usage = 0;
    }
//    var_list_index = roll_back_index;
    parser_print("<有返回值函数定义>\n");
    return zero;
}

int find_return_func(char *func_to_find) {
    int i;
    for (i = 0; i < return_func_index; i++) {
        if (strcmp(return_func[i], func_to_find) == 0) {
            return i;
        }
    }
    return -1;
}

Parser declaration_header() {
    char type[10], identifier[100], out[100];
    if ((var1 == 1 && !equal(pre_read1.content, "int") && !equal(pre_read1.content, "char"))
        || (var1 == 0 && !equal(sym.content, "int") && !equal(sym.content, "char"))) {
        return negative;
    }
    if (var1 == 1) {
        var1 = 0;
        strcpy(type, pre_read1.content);
        if (equal(pre_read1.content, "int")) {
            return_func_kind[return_func_index] = 0;
        }
        else {
            return_func_kind[return_func_index] = 1;
        }
        print_word(pre_read1);
    }
    else {
        strcpy(type, sym.content);
        if (equal(sym.content, "int")) {
            return_func_kind[return_func_index] = 0;
        }
        else {
            return_func_kind[return_func_index] = 1;
        }
        print_word(sym);
        getsym();
    }
    if ((var2 == 1 && !equal(mark[pre_read2.type], "IDENFR"))
        || (var2 == 0 && !equal(mark[sym.type], "IDENFR"))) {
        handle_error("wrong identifier!");
        return negative;
    }
    if (var2 == 1) {
        var2 = 0;
        strcpy(return_func[return_func_index++], pre_read2.content);
        strcpy(identifier, pre_read2.content);
        print_word(pre_read2);
    }
    else {
        strcpy(return_func[return_func_index++], sym.content);
        strcpy(identifier, sym.content);
        print_word(sym);
        getsym();
    }
    sprintf(out, "%s:", identifier);
    m_list[m_list_len++] = middle_code_create(LABLE, 0, 1, out);
    m_list[m_list_len++] = middle_code_create(DECLARATION_HEADER, 0, 3, type, identifier, "()");
    parser_print("<声明头部>\n");
    return zero;
}

Parser parameter_table() {
    char type[10], identifier[100];
    int l, kind;
    if (equal(sym.content, ")")) {
        parser_print("<参数表>\n");
        return zero;
    }
    if (!equal(sym.content, "int") && !equal(sym.content, "char")) {
        handle_error("no int/char in parameter_table");
        return negative;
    }
    strcpy(type, sym.content);
    kind = equal(type, "int") ? 0 : 1;
    print_word(sym);
    getsym();
    if (!equal(mark[sym.type], "IDENFR")) {
        handle_error("wrong identifier!");
        return negative;
    }
    if ((l = find_var(sym.content)) != -1) {
        sprintf(var_list[l].replace, "$temp_%d", reg_cnt++);
        var_list[l].usage = 1;
        strcpy(identifier, var_list[l].replace);
        var_list[l].replace_kind = kind;
    }
    else {
        strcpy(var_list[var_list_index].name, sym.content);
        var_list[var_list_index].kind = kind;
        var_list[var_list_index++].usage = 0;
        strcpy(identifier, sym.content);
    }
//    strcpy(identifier, sym.content);
    m_list[m_list_len++] = middle_code_create(PARAMETER, kind, 3, "para", type, identifier);
    print_word(sym);
    getsym();
    while (equal(sym.content, ",")) {
        print_word(sym);
        getsym();
        if (!equal(sym.content, "int") && !equal(sym.content, "char")) {
            handle_error("no int/char after ','");
            return negative;
        }
        strcpy(type, sym.content);
        kind = equal(type, "int") ? 0 : 1;
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return negative;
        }
        if ((l = find_var(sym.content)) != -1) {
            sprintf(var_list[l].replace, "$temp_%d", reg_cnt++);
            var_list[l].usage = 1;
            strcpy(identifier, var_list[l].replace);
            var_list[l].replace_kind = kind;
        }
        else {
            strcpy(var_list[var_list_index].name, sym.content);
            var_list[var_list_index].kind = kind;
            var_list[var_list_index++].usage = 0;
            strcpy(identifier, sym.content);
        }
//        strcpy(identifier, sym.content);
        m_list[m_list_len++] = middle_code_create(PARAMETER, kind, 3, "para", type, identifier);
        print_word(sym);
        getsym();
    }
    parser_print("<参数表>\n");
    return zero;
}

Parser compound_statement() {
    Parser r;
    check(constant_specification())
    check(variable_specification())
    check(statement_column())
    parser_print("<复合语句>\n");
    return zero;
}

Parser statement_column() {
    while (statement().ret >= 0);
    parser_print("<语句列>\n");
    return zero;
}

Parser statement() {
    Parser r;
    if (equal(sym.content, ";")) {
        print_word(sym);
        getsym();
    }
    else if (conditional_statement().ret >= 0) {}
    else if (loop_statement().ret >= 0) {}
    else if (equal(sym.content, "{")) {
        print_word(sym);
        getsym();
        check(statement_column())
        if (!equal(sym.content, "}")) {
            handle_error("no '}' after '{'");
            return negative;
        }
        print_word(sym);
        getsym();
    }
    else {
        if (func_call_statement().ret >= 0) {}
        else if (assignment_statement().ret >= 0) {}
        else if (read_statement().ret >= 0) {}
        else if (write_statement().ret >= 0) {}
        else if (return_statement().ret >= 0) {}
        else {
            return negative;
        }
        if (!equal(sym.content, ";")) {
            handle_error("no ';' after statement");
            return negative;
        }
        print_word(sym);
        getsym();
    }
    parser_print("<语句>\n");
    return zero;
}

Parser conditional_statement() {
    Parser r;
    char out[100], out_use[100], if_end[100], if_end_use[100];
    sprintf(out, "label_%d:", label);
    sprintf(out_use, "label_%d", label++);
    sprintf(if_end, "label_%d:", label);
    sprintf(if_end_use, "label_%d", label++);
    if (!equal(sym.content, "if")) {
        return negative;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, "(")) {
        handle_error("no '(' after if");
        return negative;
    }
    print_word(sym);
    getsym();
    check(condition())
    m_list[m_list_len++] = middle_code_create(JUMP_STATEMENT, 0, 2, "BZ", out_use);
    if (!equal(sym.content, ")")) {
        handle_error("no ')' after '('");
        return negative;
    }
    print_word(sym);
    getsym();
    check(statement())
    m_list[m_list_len++] = middle_code_create(JUMP_STATEMENT, 0, 2, "GOTO", if_end_use);
    //不满足条件跳转到的label设在这里
    m_list[m_list_len++] = middle_code_create(LABLE, 0, 1, out);
    if (equal(sym.content, "else")) {
        print_word(sym);
        getsym();
        check(statement())
    }
    //end_label
    m_list[m_list_len++] = middle_code_create(LABLE, 0, 1, if_end);
    parser_print("<条件语句>\n");
    return zero;
}

Parser condition() {
    Parser r;
    int flag = 0;
    char f1[100], f2[100], op[10];
    check(expression())
    strcpy(f1, r.content[0]);
    if (sym.type >= 21 && sym.type <= 26) {
        flag = 1;
        strcpy(op, sym.content);
        print_word(sym);
        getsym();
        check(expression())
        strcpy(f2, r.content[0]);
    }
    // flag == 0 没有关系运算符和第二个表达式
    if (flag == 0) {
        m_list[m_list_len++] = middle_code_create(CONDITION, 0, 3, f1, "!=", "0");
    }
    else {
        m_list[m_list_len++] = middle_code_create(CONDITION, 0, 3, f1, op, f2);
    }
    parser_print("<条件>\n");
    return zero;
}

Parser expression() {
    Parser r, ret = {0, 0, 0};
    int flag = 0;
    char f1[100], f2[100], op[10], out[100];
    if ((equal(sym.content, "+") || equal(sym.content, "-")) && sym.type != 2) {
        strcpy(op, sym.content);
        flag = 1;
        print_word(sym);
        getsym();
    }
    check(term())
    ret.ret = r.ret;
    if (flag == 0) {
        strcpy(f1, r.content[0]);
        ret.kind = r.kind;
    }
    else {
        ret.ret = 1;
        strcpy(f2, r.content[0]);
        sprintf(out, "$reg%d", reg_cnt++);
        m_list[m_list_len++] = middle_code_create(CALCULATE, 0, 5, out, "=", "0", op, f2);
        strcpy(f1, out);
    }
    while ((equal(sym.content, "+") || equal(sym.content, "-")) && sym.type != 2) {
        ret.ret = 1;
        ret.kind = 0;
        strcpy(op, sym.content);
        print_word(sym);
        getsym();
        check(term())
        strcpy(f2, r.content[0]);
        sprintf(out, "$reg%d", reg_cnt++);
        m_list[m_list_len++] = middle_code_create(CALCULATE, 0, 5, out, "=", f1, op, f2);
        strcpy(f1, out);
    }
    strcpy(ret.content[0], f1);
    parser_print("<表达式>\n");
    return ret;
}

Parser term() {
    Parser r, ret = {0};
    char f1[100], f2[100], op[10], out[100];
    check(factor())
    strcpy(f1, r.content[0]);
    ret.kind = r.kind;
    ret.ret = r.ret;
    while ((equal(sym.content, "*") || equal(sym.content, "/")) && sym.type != 2) {
        ret.ret = 1;
        ret.kind = 0;
        strcpy(op, sym.content);
        print_word(sym);
        getsym();
        check(factor())
        strcpy(f2, r.content[0]);
        sprintf(out, "$reg%d", reg_cnt++);
        m_list[m_list_len++] = middle_code_create(CALCULATE, 0, 5, out, "=", f1, op, f2);
        strcpy(f1, out);
    }
    strcpy(ret.content[0], f1);
    parser_print("<项>\n");
    return ret;
}

Parser factor() {
    Parser r, ret = {0, 1, 0};
    char identifier[100], char_con[10], f1[100];
    int l;
    if ((r = func_call_statement()).ret >= 0) {
        strcpy(ret.content[0], r.content[0]);
        ret.kind = r.kind;
        ret.ret = 2;
    }
    else if (equal(mark[sym.type], "IDENFR")) {
        l = find_var(sym.content);
        if (var_list[l].usage == 1) {
            strcpy(identifier, var_list[l].replace);
            ret.kind = var_list[l].replace_kind;
        }
        else {
            strcpy(identifier, sym.content);
            ret.kind = var_list[l].kind;
        }
        print_word(sym);
        getsym();
        if (equal(sym.content, "[")) {
            print_word(sym);
            getsym();
            check(expression())
            strcpy(f1, r.content[0]);
            if (!equal(sym.content, "]")) {
                handle_error("no ']' after '['");
                return negative;
            }
            sprintf(ret.content[0], "%s[%s]", identifier, f1);
            print_word(sym);
            getsym();
        }
        else {
            strcpy(ret.content[0], identifier);
        }
    }
    else if (equal(sym.content, "(")) {
        print_word(sym);
        getsym();
        check(expression())
        if (!equal(sym.content, ")")) {
            handle_error("no ')' after ')'");
            return negative;
        }
        strcpy(ret.content[0], r.content[0]);
        print_word(sym);
        getsym();
    }
    else if ((r = integer()).ret >= 0) {
        strcpy(ret.content[0], r.content[0]);
    }
    else if (equal(mark[sym.type], "CHARCON")) {
        sprintf(char_con, "\'%s\'", sym.content);
        strcpy(ret.content[0], char_con);
        ret.kind = 1;
        print_word(sym);
        getsym();
    }
    else {
        //print_word(sym);
        handle_error("no matched factor");
        return negative;
    }
    parser_print("<因子>\n");
    return ret;
}

Parser func_call_statement() {
    Parser r, ret = {0, 0, 0};
    char func[1000], out[100];
    int l;
    if (!equal(mark[sym.type], "IDENFR")) {
        return negative;
    }
    strcpy(func, sym.content);
    if (find_return_func(func) == -1 && find_no_return_func(func) == -1) {
        return negative;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, "(")) {
        handle_error("no '(' after func_call");
        return negative;
    }
    print_word(sym);
    getsym();
    check(value_parameter_table())
    if (!equal(sym.content, ")")) {
        handle_error("no ')' after '('");
        return negative;
    }
    print_word(sym);
    getsym();
    if ((l = find_return_func(func)) != -1) {
        parser_print("<有返回值函数调用语句>\n");
        ret.kind = return_func_kind[l];
        m_list[m_list_len++] = middle_code_create(FUNCTION_CALL, 0, 2, "call", func);
        sprintf(out, "$reg%d", reg_cnt++);
        m_list[m_list_len++] = middle_code_create(ASSIGNMENT_STATEMENT, ret.kind, 3, out, "=", "RET");
        strcpy(ret.content[0], out);
    }
    else if (find_no_return_func(func) != -1) {
        parser_print("<无返回值函数调用语句>\n");
        m_list[m_list_len++] = middle_code_create(FUNCTION_CALL, 0, 2, "call", func);
    }
    else {
        handle_error("no such func in func_call");
        return negative;
    }
    return ret;
}

Parser value_parameter_table() {
    Parser r;
    char list[100][100];
    int list_index = 0, i;
    if (equal(sym.content, ")")) {
        parser_print("<值参数表>\n");
        return zero;
    }
    check(expression())
//    m_list[m_list_len++] = middle_code_create(FUNCTION_CALL_PARAMETER, 0, 2, "push", r.content[0]);
    strcpy(list[list_index++], r.content[0]);
    while (equal(sym.content, ",")) {
        print_word(sym);
        getsym();
        check(expression())
//        m_list[m_list_len++] = middle_code_create(FUNCTION_CALL_PARAMETER, 0, 2, "push", r.content[0]);
        strcpy(list[list_index++], r.content[0]);
    }
    for (i = 0; i < list_index; i++) {
        m_list[m_list_len++] = middle_code_create(FUNCTION_CALL_PARAMETER, 0, 2, "push", list[i]);
    }
    parser_print("<值参数表>\n");
    return zero;
}

Parser loop_statement() {
    Parser r;
    char begin_label[100], end_label[100], begin_label_use[100], end_label_use[100];
    char identifier[100], out[100], f1[100], op[10], f2[100];
    int l;
    Middle_code move;
    if (equal(sym.content, "while")) {
        //while_bigin:
        sprintf(begin_label, "label_%d:", label);
        sprintf(begin_label_use, "label_%d", label++);
        sprintf(end_label, "label_%d:", label);
        sprintf(end_label_use, "label_%d", label++);
        m_list[m_list_len++] = middle_code_create(LABLE, 0, 1, begin_label);
        print_word(sym);
        getsym();
        if (!equal(sym.content, "(")) {
            handle_error("no '(' after while");
            return negative;
        }
        print_word(sym);
        getsym();
        check(condition())
        if (!equal(sym.content, ")")) {
            handle_error("no ')' after '(' in while");
            return negative;
        }
        //不满足条件跳转至while_end
        m_list[m_list_len++] = middle_code_create(JUMP_STATEMENT, 0, 2, "BZ", end_label_use);
        print_word(sym);
        getsym();
        check(statement())
        //无条件跳转至while_begin
        m_list[m_list_len++] = middle_code_create(JUMP_STATEMENT, 0, 2, "GOTO", begin_label_use);
        //while_end:
        m_list[m_list_len++] = middle_code_create(LABLE, 0, 1, end_label);
    }
    else if (equal(sym.content, "do")) {
        //do_while_begin:
        sprintf(begin_label, "label_%d:", label);
        sprintf(begin_label_use, "label_%d", label++);
        sprintf(end_label, "label_%d:", label);
        sprintf(end_label_use, "label_%d", label++);
        m_list[m_list_len++] = middle_code_create(LABLE, 0, 1, begin_label);
        print_word(sym);
        getsym();
        check(statement())
        if (!equal(sym.content, "while")) {
            handle_error("no while after do");
            return negative;
        }
        print_word(sym);
        getsym();
        if (!equal(sym.content, "(")) {
            handle_error("no '(' after do...while");
            return negative;
        }
        print_word(sym);
        getsym();
        check(condition())
        if (!equal(sym.content, ")")) {
            handle_error("no ')' after '(' in do...while");
            return negative;
        }
        print_word(sym);
        getsym();
        //满足条件跳转至do_while_begin
        m_list[m_list_len++] = middle_code_create(JUMP_STATEMENT, 0, 2, "BNZ", begin_label_use);
        //do_while_end:
        m_list[m_list_len++] = middle_code_create(LABLE, 0, 1, end_label);
    }
    else if (equal(sym.content, "for")) {
        sprintf(begin_label, "label_%d:", label);
        sprintf(begin_label_use, "label_%d", label++);
        sprintf(end_label, "label_%d:", label);
        sprintf(end_label_use, "label_%d", label++);
        print_word(sym);
        getsym();
        if (!equal(sym.content, "(")) {
            handle_error("no '(' after for");
            return negative;
        }
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return negative;
        }
        l = find_var(sym.content);
        if (var_list[l].usage == 1) {
            strcpy(identifier, var_list[l].replace);
        }
        else {
            strcpy(identifier, sym.content);
        }
        print_word(sym);
        getsym();
        if (!equal(sym.content, "=")) {
            handle_error("no '=' after for");
            return negative;
        }
        print_word(sym);
        getsym();
        check(expression())
        //把赋值语句里面的东西拿过来了
        strcpy(f1, r.content[0]);
        if (r.ret == 1) {
            strcpy(m_list[m_list_len - 1].code[0], identifier);
        }
        else {
            m_list[m_list_len++] = middle_code_create(ASSIGNMENT_STATEMENT, 0, 3, identifier, "=", f1);
        }
        //上面是搬过来的东西
        if (!equal(sym.content, ";")) {
            handle_error("no ';' after for");
            return negative;
        }
        print_word(sym);
        getsym();
        //for_begin:
        m_list[m_list_len++] = middle_code_create(LABLE, 0, 1, begin_label);
        check(condition())
        //不满足条件跳转至for_end
        m_list[m_list_len++] = middle_code_create(JUMP_STATEMENT, 0, 2, "BZ", end_label_use);
        if (!equal(sym.content, ";")) {
            handle_error("no ';' after for(;");
            return negative;
        }
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return negative;
        }
        l = find_var(sym.content);
        if (var_list[l].usage == 1) {
            strcpy(out, var_list[l].replace);
        }
        else {
            strcpy(out, sym.content);
        }
//        strcpy(out, sym.content);
        print_word(sym);
        getsym();
        if (!equal(sym.content, "=")) {
            handle_error("no '=' after for(;;");
            return negative;
        }
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return negative;
        }
        l = find_var(sym.content);
        if (var_list[l].usage == 1) {
            strcpy(f1, var_list[l].replace);
        }
        else {
            strcpy(f1, sym.content);
        }
//        strcpy(f1, sym.content);
        print_word(sym);
        getsym();
        if (!equal(sym.content, "+") && !equal(sym.content, "-")) {
            handle_error("no '+'/'-' after for(;;");
            return negative;
        }
        strcpy(op, sym.content);
        print_word(sym);
        getsym();
        check(step())
        strcpy(f2, r.content[0]);
        //直接当一个计算语句处理
        move = middle_code_create(CALCULATE, 0, 5, out, "=", f1, op, f2);
        if (!equal(sym.content, ")")) {
            handle_error("no ')' after '(' in for");
            return negative;
        }
        print_word(sym);
        getsym();
        check(statement())
        //无条件跳转至for_begin
        m_list[m_list_len++] = move;
        m_list[m_list_len++] = middle_code_create(JUMP_STATEMENT, 0, 2, "GOTO", begin_label_use);
        //faor_end:
        m_list[m_list_len++] = middle_code_create(LABLE, 0, 1, end_label);
    }
    else {
        return negative;
    }
    parser_print("<循环语句>\n");
    return zero;
}

Parser step() {
    Parser r;
    check(unsigned_integer())
    parser_print("<步长>\n");
    return r;
}

Parser assignment_statement() {
    Parser r;
    int l;
    char identifier[100], out[100], f1[100];
    if (!equal(mark[sym.type], "IDENFR")) {
        return negative;
    }
    l = find_var(sym.content);
    if (var_list[l].usage == 1) {
        strcpy(identifier, var_list[l].replace);
    }
    else {
        strcpy(identifier, sym.content);
    }
//    strcpy(identifier, sym.content);
    print_word(sym);
    getsym();
    if (equal(sym.content, "[")) {
        print_word(sym);
        getsym();
        check(expression())
        strcpy(f1, r.content[0]);
        if (!equal(sym.content, "]")) {
            handle_error("no ']' after '[' in assignment_statement");
            return negative;
        }
        sprintf(out, "%s[%s]", identifier, f1);
        strcpy(identifier, out);
        print_word(sym);
        getsym();
    }
    if (!equal(sym.content, "=")) {
        handle_error("no '=' or '[' in assignment_statement");
        return negative;
    }
    print_word(sym);
    getsym();
    check(expression())
    strcpy(f1, r.content[0]);
    if (r.ret == 1 || r.ret == 2) {
        strcpy(m_list[m_list_len - 1].code[0], identifier);
    }
    else {
        m_list[m_list_len++] = middle_code_create(ASSIGNMENT_STATEMENT, r.kind, 3, identifier, "=", f1);
    }
    parser_print("<赋值语句>\n");
    return zero;
}

Parser read_statement() {
    char identifier[100];
    int l, kind;
    if (!equal(sym.content, "scanf")) {
        return negative;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, "(")) {
        handle_error("no '(' after scanf");
        return negative;
    }
    print_word(sym);
    getsym();
    if (!equal(mark[sym.type], "IDENFR")) {
        handle_error("wrong identifier!");
        return negative;
    }
    l = find_var(sym.content);
    if (var_list[l].usage == 1) {
        strcpy(identifier, var_list[l].replace);
        kind = var_list[l].replace_kind;
    }
    else {
        strcpy(identifier, sym.content);
        kind = var_list[l].kind;
    }
    m_list[m_list_len++] = middle_code_create(READ_STATEMENT, kind, 2, "scanf", identifier);
    print_word(sym);
    getsym();
    while (equal(sym.content, ",")) {
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return negative;
        }
        l = find_var(sym.content);
        if (var_list[l].usage == 1) {
            strcpy(identifier, var_list[l].replace);
            kind = var_list[l].replace_kind;
        }
        else {
            strcpy(identifier, sym.content);
            kind = var_list[l].kind;
        }
        m_list[m_list_len++] = middle_code_create(READ_STATEMENT, kind, 2, "scanf", identifier);
        print_word(sym);
        getsym();
    }
    if (!equal(sym.content, ")")) {
        handle_error("no ')' after '(' in scanf");
        return negative;
    }
    print_word(sym);
    getsym();
    parser_print("<读语句>\n");
    return zero;
}

Parser write_statement() {
    Parser r;
    char str[1000];
    int kind;
    if (!equal(sym.content, "printf")) {
        return negative;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, "(")) {
        handle_error("no '(' after printf");
        return negative;
    }
    print_word(sym);
    getsym();
    r = string();
    strcpy(str, r.content[0]);
    if (r.ret >= 0) {
        if (equal(sym.content, ",")) {
            print_word(sym);
            getsym();
            check(expression())
            kind = r.kind;
            m_list[m_list_len++] = middle_code_create(WRITE_STATEMENT_1, kind, 3, "write", str, r.content[0]);
        }
        else {
            m_list[m_list_len++] = middle_code_create(WRITE_STATEMENT_2, 0, 2, "write", str);
        }
    }
    else {
        r = expression();
        if (r.ret >= 0) {
            kind = r.kind;
            m_list[m_list_len++] = middle_code_create(WRITE_STATEMENT_3, kind, 2, "write", r.content[0]);
        }
        else {
            handle_error("printf content error");
            return negative;
        }
    }
    if (!equal(sym.content, ")")) {
        handle_error("no ')' after '(' in printf");
        return negative;
    }
    print_word(sym);
    getsym();
    parser_print("<写语句>\n");
    return zero;
}

Parser string() {
    Parser ret = {0, 1};
    if (!equal(mark[sym.type], "STRCON")) {
        return negative;
    }
    strcpy(ret.content[0], sym.content);
    print_word(sym);
    getsym();
    parser_print("<字符串>\n");
    return ret;
}

Parser return_statement() {
    Parser r;
    char out[100];
    if (!equal(sym.content, "return")) {
        return negative;
    }
    print_word(sym);
    getsym();
    if (equal(sym.content, "(")) {
        print_word(sym);
        getsym();
        check(expression())
        strcpy(out, r.content[0]);
        if (!equal(sym.content, ")")) {
            handle_error("no ')' after '(' in return_statement");
            return negative;
        }
        m_list[m_list_len++] = middle_code_create(RETURN_STATEMENT, 0, 2, "ret", out);
        print_word(sym);
        getsym();
    }
    else {
        m_list[m_list_len++] = middle_code_create(RETURN_STATEMENT, 0, 1, "ret");
    }
    parser_print("<返回语句>\n");
    return zero;
}

Parser no_return_func_definition() {
    Parser r;
    int i;
    char out[100];
//    roll_back_index = var_list_index;
    if (!equal(sym.content, "void")) {
        handle_error("no 'void' in no_return_func_definition");
        return negative;
    }
    strcpy(pre_read1.content, sym.content);
    pre_read1.type = sym.type;
    var1 = 1;
    getsym();
    if (equal(sym.content, "main")) {
        return negative;
    }
    sprintf(out, "%s:", sym.content);
    m_list[m_list_len++] = middle_code_create(LABLE, 0, 1, out);
    m_list[m_list_len++] = middle_code_create(DECLARATION_HEADER, 0, 3, "void", sym.content, "()");
    print_word(pre_read1);
    var1 = 0;
    strcpy(no_return_func[no_return_func_index++], sym.content);
    print_word(sym);
    getsym();
    if (!equal(sym.content, "(")) {
        handle_error("no '(' after func header");
        return negative;
    }
    print_word(sym);
    getsym();
    check(parameter_table())
    if (!equal(sym.content, ")")) {
        handle_error("no ')' after '('");
        return negative;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, "{")) {
        handle_error("no '{' after func header");
        return negative;
    }
    print_word(sym);
    getsym();
    check(compound_statement())
    if (!equal(sym.content, "}")) {
        handle_error("no '}' after '{'");
        return negative;
    }
    print_word(sym);
    getsym();
    m_list[m_list_len++] = middle_code_create(RETURN_STATEMENT, 0, 1, "ret");
    for (i = 0; i < var_list_index; i++) {
        var_list[i].usage = 0;
    }
//    var_list_index = roll_back_index;
    parser_print("<无返回值函数定义>\n");
    return zero;
}

int find_no_return_func(char *func_to_find) {
    int i;
    for (i = 0; i < no_return_func_index; i++) {
        if (strcmp(no_return_func[i], func_to_find) == 0) {
            return i;
        }
    }
    return -1;
}

Parser main_func() {
    Parser r;
    if ((var1 == 1 && !equal(pre_read1.content, "void")) || (var1 == 0 && !equal(sym.content, "void"))) {
        handle_error("no 'void' in main_func");
        return negative;
    }
    if (var1 == 1) {
        var1 = 0;
        print_word(pre_read1);
    }
    else {
        print_word(sym);
        getsym();
    }
    if (!equal(sym.content, "main")) {
        handle_error("no 'main' in main_func");
        return negative;
    }
    main_location = m_list_len;
    m_list[m_list_len++] = middle_code_create(LABLE, 0, 1, "main:");
    m_list[m_list_len++] = middle_code_create(MAIN_FUNC, 0, 3, "void", "main", "()");
    print_word(sym);
    getsym();
    if (!equal(sym.content, "(")) {
        handle_error("no '(' after func header");
        return negative;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, ")")) {
        handle_error("no ')' after '('");
        return negative;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, "{")) {
        handle_error("no '{' after func header");
        return negative;
    }
    print_word(sym);
    getsym();
    check(compound_statement())
    if (!equal(sym.content, "}")) {
        handle_error("no '}' after '{'");
        return negative;
    }
    print_word(sym);
    parser_print("<主函数>\n");
    //m_list[m_list_len++] = middle_code_create(LABLE, 1, "end_main:");
    return zero;
}