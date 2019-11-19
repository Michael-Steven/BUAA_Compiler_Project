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
char return_func[100][1000] = {};
char no_return_func[100][1000] = {};
int return_func_index = 0;
int no_return_func_index = 0;
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
    if (equal(sym.content, "int")) {
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return negative;
        }
        strcpy(identifier, sym.content);
        print_word(sym);
        getsym();
        if (!equal(sym.content, "=")) {
            handle_error("no '=' behind const int");
            return negative;
        }
        print_word(sym);
        getsym();
        check(integer())
        m_list[m_list_len++] = middle_code_create(
                CONSTANT_SPECIFICATION, 5, "const", "int", identifier, "=", r.content[0]);
        while (equal(sym.content, ",")) {
            print_word(sym);
            getsym();
            if (!equal(mark[sym.type], "IDENFR")) {
                handle_error("wrong identifier!");
                return negative;
            }
            strcpy(identifier, sym.content);
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
                    CONSTANT_SPECIFICATION, 5, "const", "int", identifier, "=", r.content[0]);
        }
        parser_print("<常量定义>\n");
        return zero;
    }
    if (equal(sym.content, "char")) {
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return negative;
        }
        strcpy(identifier, sym.content);
        print_word(sym);
        getsym();
        if (!equal(sym.content, "=")) {
            handle_error("no '=' behind const char");
            return negative;
        }
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "CHARCON")) {
            handle_error("wrong character");
            return negative;
        }
        m_list[m_list_len++] = middle_code_create(
                CONSTANT_SPECIFICATION, 5, "const", "char", identifier, "=", sym.content);
        print_word(sym);
        getsym();
        while (equal(sym.content, ",")) {
            print_word(sym);
            getsym();
            if (!equal(mark[sym.type], "IDENFR")) {
                handle_error("wrong identifier!");
                return negative;
            }
            strcpy(identifier, sym.content);
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
                    CONSTANT_SPECIFICATION, 5, "const", "char", identifier, "=", sym.content);
            print_word(sym);
            getsym();
        }
        parser_print("<常量定义>\n");
        return zero;
    }
    return negative;
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
    Parser ret = {0, 0};
    if (!equal(mark[sym.type], "INTCON")) {
        return negative;
    }
    strcpy(ret.content[ret.c_len++], sym.content);
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
    char type[10], identifier[100];
    strcpy(type, pre_read1.content);
    print_word(pre_read1);
    var1 = 0;
    if (!equal(mark[pre_read2.type], "IDENFR")) {
        handle_error("wrong identifier!");
        return negative;
    }
    strcpy(identifier, pre_read2.content);
    print_word(pre_read2);
    var2 = 0;
    if (equal(sym.content, "[")) {
        print_word(sym);
        getsym();
        if (equal(unsigned_integer().content[0], "0")) {
            handle_error("number of array is 0");
            return negative;
        }
        if (!equal(sym.content, "]")) {
            handle_error("no ']' after '['");
            return negative;
        }
        print_word(sym);
        getsym();
    }
    else {
        m_list[m_list_len++] = middle_code_create(VARIABLE_SPECIFICATION, 3, "var", type, identifier);
    }
    while (equal(sym.content, ",")) {
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return negative;
        }
        strcpy(identifier, sym.content);
        print_word(sym);
        getsym();
        if (equal(sym.content, "[")) {
            print_word(sym);
            getsym();
            if (equal(unsigned_integer().content[0], "0")) {
                handle_error("number of array is 0");
                return negative;
            }
            if (!equal(sym.content, "]")) {
                handle_error("no ']' after '['");
                return negative;
            }
            print_word(sym);
            getsym();
        }
        else {
            m_list[m_list_len++] = middle_code_create(VARIABLE_SPECIFICATION, 3, "var", type, identifier);
        }
    }
    parser_print("<变量定义>\n");
    return zero;
}

Parser return_func_definition() {
    Parser r;
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
    parser_print("<有返回值函数定义>\n");
    return zero;
}

int find_return_func(char *func_to_find) {
    int i;
    for (i = 0; i < return_func_index; i++) {
        if (strcmp(return_func[i], func_to_find) == 0) {
            return 1;
        }
    }
    return 0;
}

Parser declaration_header() {
    if ((var1 == 1 && !equal(pre_read1.content, "int") && !equal(pre_read1.content, "char"))
        || (var1 == 0 && !equal(sym.content, "int") && !equal(sym.content, "char"))) {
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
    if ((var2 == 1 && !equal(mark[pre_read2.type], "IDENFR"))
        || (var2 == 0 && !equal(mark[sym.type], "IDENFR"))) {
        handle_error("wrong identifier!");
        return negative;
    }
    if (var2 == 1) {
        var2 = 0;
        strcpy(return_func[return_func_index++], pre_read2.content);
        print_word(pre_read2);
    }
    else {
        strcpy(return_func[return_func_index++], sym.content);
        print_word(sym);
        getsym();
    }
    parser_print("<声明头部>\n");
    return zero;
}

Parser parameter_table() {
    if (equal(sym.content, ")")) {
        parser_print("<参数表>\n");
        return zero;
    }
    if (!equal(sym.content, "int") && !equal(sym.content, "char")) {
        handle_error("no int/char in parameter_table");
        return negative;
    }
    print_word(sym);
    getsym();
    if (!equal(mark[sym.type], "IDENFR")) {
        handle_error("wrong identifier!");
        return negative;
    }
    print_word(sym);
    getsym();
    while (equal(sym.content, ",")) {
        print_word(sym);
        getsym();
        if (!equal(sym.content, "int") && !equal(sym.content, "char")) {
            handle_error("no int/char after ','");
            return negative;
        }
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return negative;
        }
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
    if (!equal(sym.content, ")")) {
        handle_error("no ')' after '('");
        return negative;
    }
    print_word(sym);
    getsym();
    check(statement())
    if (equal(sym.content, "else")) {
        print_word(sym);
        getsym();
        check(statement())
    }
    parser_print("<条件语句>\n");
    return zero;
}

Parser condition() {
    Parser r;
    check(expression())
    if (sym.type >= 21 && sym.type <= 26) {
        print_word(sym);
        getsym();
        check(expression())
    }
    parser_print("<条件>\n");
    return zero;
}

Parser expression() {
    Parser r, ret = {0, 1};
    int flag = 0;
    char f1[100], f2[100], op[10], out[100];
    if ((equal(sym.content, "+") || equal(sym.content, "-")) && sym.type != 2) {
        strcpy(op, sym.content);
        flag = 1;
        print_word(sym);
        getsym();
    }
    check(term())
    if (flag == 0) {
        strcpy(f1, r.content[0]);
    }
    else {
        strcpy(f2, r.content[0]);
        sprintf(out, "s%d", reg_cnt++);
        m_list[m_list_len++] = middle_code_create(CALCULATE, 5, out, "=", "0", op, f2);
        strcpy(f1, out);
    }
    while ((equal(sym.content, "+") || equal(sym.content, "-")) && sym.type != 2) {
        ret.ret = 1;
        strcpy(op, sym.content);
        print_word(sym);
        getsym();
        check(term())
        strcpy(f2, r.content[0]);
        sprintf(out, "s%d", reg_cnt++);
        m_list[m_list_len++] = middle_code_create(CALCULATE, 5, out, "=", f1, op, f2);
        strcpy(f1, out);
    }
    strcpy(ret.content[0], f1);
    parser_print("<表达式>\n");
    return ret;
}

Parser term() {
    Parser r, ret = {0, 1};
    char f1[100], f2[100], op[10], out[100];
    check(factor())
    strcpy(f1, r.content[0]);
    while ((equal(sym.content, "*") || equal(sym.content, "/")) && sym.type != 2) {
        strcpy(op, sym.content);
        print_word(sym);
        getsym();
        check(factor())
        strcpy(f2, r.content[0]);
        sprintf(out, "s%d", reg_cnt++);
        m_list[m_list_len++] = middle_code_create(CALCULATE, 5, out, "=", f1, op, f2);
        strcpy(f1, out);
    }
    strcpy(ret.content[0], f1);
    parser_print("<项>\n");
    return ret;
}

Parser factor() {
    Parser r, ret = {0, 1, 1};
    char identifier[100], char_con[10];
    if (func_call_statement().ret >= 0) {}
    else if (equal(mark[sym.type], "IDENFR")) {
        strcpy(identifier, sym.content);
        print_word(sym);
        getsym();
        if (equal(sym.content, "[")) {
            print_word(sym);
            getsym();
            check(expression())
            if (!equal(sym.content, "]")) {
                handle_error("no ']' after '['");
                return negative;
            }
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
    Parser r;
    char func[1000];
    if (!equal(mark[sym.type], "IDENFR")) {
        return negative;
    }
    strcpy(func, sym.content);
    if (!find_return_func(func) && !find_no_return_func(func)) {
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
    if (find_return_func(func)) {
        parser_print("<有返回值函数调用语句>\n");
    }
    else if (find_no_return_func(func)) {
        parser_print("<无返回值函数调用语句>\n");
    }
    else {
        handle_error("no such func in func_call");
        return negative;
    }
    return zero;
}

Parser value_parameter_table() {
    Parser r;
    if (equal(sym.content, ")")) {
        parser_print("<值参数表>\n");
        return zero;
    }
    check(expression())
    while (equal(sym.content, ",")) {
        print_word(sym);
        getsym();
        check(expression())
    }
    parser_print("<值参数表>\n");
    return zero;
}

Parser loop_statement() {
    Parser r;
    if (equal(sym.content, "while")) {
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
        print_word(sym);
        getsym();
        check(statement())
    }
    else if (equal(sym.content, "do")) {
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
    }
    else if (equal(sym.content, "for")) {
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
        print_word(sym);
        getsym();
        if (!equal(sym.content, "=")) {
            handle_error("no '=' after for");
            return negative;
        }
        print_word(sym);
        getsym();
        check(expression())
        if (!equal(sym.content, ";")) {
            handle_error("no ';' after for");
            return negative;
        }
        print_word(sym);
        getsym();
        check(condition())
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
        print_word(sym);
        getsym();
        if (!equal(sym.content, "+") && !equal(sym.content, "-")) {
            handle_error("no '+'/'-' after for(;;");
            return negative;
        }
        print_word(sym);
        getsym();
        check(step())
        if (!equal(sym.content, ")")) {
            handle_error("no ')' after '(' in for");
            return negative;
        }
        print_word(sym);
        getsym();
        check(statement())
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
    return zero;
}

Parser assignment_statement() {
    Parser r;
    char identifier[100], f1[100];
    if (!equal(mark[sym.type], "IDENFR")) {
        return negative;
    }
    strcpy(identifier, sym.content);
    print_word(sym);
    getsym();
    if (equal(sym.content, "[")) {
        print_word(sym);
        getsym();
        check(expression())
        if (!equal(sym.content, "]")) {
            handle_error("no ']' after '[' in assignment_statement");
            return negative;
        }
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
    if (r.ret == 1) {
        strcpy(m_list[m_list_len - 1].code[0], identifier);
    }
    else {
        m_list[m_list_len++] = middle_code_create(ASSIGNMENT_STATEMENT, 3, identifier, "=", f1);
    }
    parser_print("<赋值语句>\n");
    return zero;
}

Parser read_statement() {
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
    m_list[m_list_len++] = middle_code_create(READ_STATEMENT, 2, "read", sym.content);
    print_word(sym);
    getsym();
    while (equal(sym.content, ",")) {
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return negative;
        }
        m_list[m_list_len++] = middle_code_create(READ_STATEMENT, 2, "read", sym.content);
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
            m_list[m_list_len++] = middle_code_create(WRITE_STATEMENT_1, 3, "write", str, r.content[0]);
        }
        else {
            m_list[m_list_len++] = middle_code_create(WRITE_STATEMENT_2, 2, "write", str);
        }
    }
    else {
        r = expression();
        if (r.ret >= 0) {
            m_list[m_list_len++] = middle_code_create(WRITE_STATEMENT_3, 2, "write", r.content[0]);
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
    if (!equal(sym.content, "return")) {
        return negative;
    }
    print_word(sym);
    getsym();
    if (equal(sym.content, "(")) {
        print_word(sym);
        getsym();
        check(expression())
        if (!equal(sym.content, ")")) {
            handle_error("no ')' after '(' in return_statement");
            return negative;
        }
        print_word(sym);
        getsym();
    }
    parser_print("<返回语句>\n");
    return zero;
}

Parser no_return_func_definition() {
    Parser r;
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
    parser_print("<无返回值函数定义>\n");
    return zero;
}

int find_no_return_func(char *func_to_find) {
    int i;
    for (i = 0; i < no_return_func_index; i++) {
        if (strcmp(no_return_func[i], func_to_find) == 0) {
            return 1;
        }
    }
    return 0;
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
    m_list[m_list_len++] = middle_code_create(MAIN_FUNC, 2, "void", "main");
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