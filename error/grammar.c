#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "word.h"
#include "grammar.h"
#include "error.h"

int main() {
    freopen("testfile.txt", "r", stdin);
    freopen("error.txt", "w", stdout);
    c = getchar();
    getsym();
    if (program() < 0) {
        //printf("compile error\n");
    }
    return 0;
}

int program() {
    int r;
    check(constant_specification())
    check(variable_specification())
    while (1) {
        if (return_func_definition() < 0) {
            if (no_return_func_definition() < 0) {
                break;
            }
        }
    }
    check(main_func())
    //printf("<程序>\n");
    return 0;
}

int constant_specification() {
    int r;
    if (!equal(sym.content, "const")) {
        return 0;
    }
    while (equal(sym.content, "const")) {
        print_word(sym);
        getsym();
        check(constant_definition())
        if (!equal(sym.content, ";")) {
            handle_error("no ';' behind const definition");
            return -1;
        }
        print_word(sym);
        getsym();
    }
    //printf("<常量说明>\n");
    return 0;
}

int constant_definition() {
    int r;
    if (equal(sym.content, "int")) {
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return -1;
        }
        print_word(sym);
        getsym();
        if (!equal(sym.content, "=")) {
            handle_error("no '=' behind const int");
            return -1;
        }
        print_word(sym);
        getsym();
        check(integer())
        while (equal(sym.content, ",")) {
            print_word(sym);
            getsym();
            if (!equal(mark[sym.type], "IDENFR")) {
                handle_error("wrong identifier!");
                return -1;
            }
            print_word(sym);
            getsym();
            if (!equal(sym.content, "=")) {
                handle_error("no '=' behind const int, int");
                return -1;
            }
            print_word(sym);
            getsym();
            check(integer())
        }
        //printf("<常量定义>\n");
        return 0;
    }
    if (equal(sym.content, "char")) {
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return -1;
        }
        print_word(sym);
        getsym();
        if (!equal(sym.content, "=")) {
            handle_error("no '=' behind const char");
            return -1;
        }
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "CHARCON")) {
            handle_error("wrong character");
            return -1;
        }
        print_word(sym);
        getsym();
        while (equal(sym.content, ",")) {
            print_word(sym);
            getsym();
            if (!equal(mark[sym.type], "IDENFR")) {
                handle_error("wrong identifier!");
                return -1;
            }
            print_word(sym);
            getsym();
            if (!equal(sym.content, "=")) {
                handle_error("no '=' behind const char, char");
                return -1;
            }
            print_word(sym);
            getsym();
            if (!equal(mark[sym.type], "CHARCON")) {
                handle_error("wrong character");
                return -1;
            }
            print_word(sym);
            getsym();
        }
        //printf("<常量定义>\n");
        return 0;
    }
    return -1;
}

int integer() {
    int r;
    if ((equal(sym.content, "+") || equal(sym.content, "-")) && sym.type != 2) {
        print_word(sym);
        getsym();
    }
    check(unsigned_integer())
    //printf("<整数>\n");
    return 0;
}

int unsigned_integer() {
    int u_int;
    if (!equal(mark[sym.type], "INTCON")) {
        return -1;
    }
    print_word(sym);
    u_int = atoi(sym.content);
    getsym();
    //printf("<无符号整数>\n");
    return u_int;
}

int variable_specification() {
    int r, cnt = 0;
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
            return -1;
        }
        print_word(sym);
        getsym();
    }
    if (cnt == 0) {
        return 0;
    }
    //printf("<变量说明>\n");
    return 0;
}

int variable_definition() {
    print_word(pre_read1);
    var1 = 0;
    if (!equal(mark[pre_read2.type], "IDENFR")) {
        handle_error("wrong identifier!");
        return -1;
    }
    print_word(pre_read2);
    var2 = 0;
    if (equal(sym.content, "[")) {
        print_word(sym);
        getsym();
        if (unsigned_integer() == 0) {
            handle_error("number of array is 0");
            return -1;
        }
        if (!equal(sym.content, "]")) {
            handle_error("no ']' after '['");
            return -1;
        }
        print_word(sym);
        getsym();
    }
    while (equal(sym.content, ",")) {
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return -1;
        }
        print_word(sym);
        getsym();
        if (equal(sym.content, "[")) {
            print_word(sym);
            getsym();
            if (unsigned_integer() == 0) {
                handle_error("number of array is 0");
                return -1;
            }
            if (!equal(sym.content, "]")) {
                handle_error("no ']' after '['");
                return -1;
            }
            print_word(sym);
            getsym();
        }
    }
    //printf("<变量定义>\n");
    return 0;
}

int return_func_definition() {
    int r;
    check(declaration_header())
    if (!equal(sym.content, "(")) {
        handle_error("no '(' after func header");
        return -1;
    }
    print_word(sym);
    getsym();
    check(parameter_table())
    if (!equal(sym.content, ")")) {
        handle_error("no ')' after '('");
        return -1;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, "{")) {
        handle_error("no '{' after func header");
        return -1;
    }
    print_word(sym);
    getsym();
    check(compound_statement())
    if (!equal(sym.content, "}")) {
        handle_error("no '}' after '{'");
        return -1;
    }
    print_word(sym);
    getsym();
    //printf("<有返回值函数定义>\n");
    return 0;
}

int find_return_func(char *func_to_find) {
    int r;
    for (r = 0; r < return_func_index; r++) {
        if (strcmp(return_func[r], func_to_find) == 0) {
            return 1;
        }
    }
    return 0;
}

int declaration_header() {
    if ((var1 == 1 && !equal(pre_read1.content, "int") && !equal(pre_read1.content, "char"))
        || (var1 == 0 && !equal(sym.content, "int") && !equal(sym.content, "char"))) {
        return -1;
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
        return -1;
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
    //printf("<声明头部>\n");
    return 0;
}

int parameter_table() {
    if (equal(sym.content, ")")) {
        //printf("<参数表>\n");
        return 0;
    }
    if (!equal(sym.content, "int") && !equal(sym.content, "char")) {
        handle_error("no int/char in parameter_table");
        return -1;
    }
    print_word(sym);
    getsym();
    if (!equal(mark[sym.type], "IDENFR")) {
        handle_error("wrong identifier!");
        return -1;
    }
    print_word(sym);
    getsym();
    while (equal(sym.content, ",")) {
        print_word(sym);
        getsym();
        if (!equal(sym.content, "int") && !equal(sym.content, "char")) {
            handle_error("no int/char after ','");
            return -1;
        }
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return -1;
        }
        print_word(sym);
        getsym();
    }
    //printf("<参数表>\n");
    return 0;
}

int compound_statement() {
    int r;
    check(constant_specification())
    check(variable_specification())
    check(statement_column())
    //printf("<复合语句>\n");
    return 0;
}

int statement_column() {
    while (statement() >= 0);
    //printf("<语句列>\n");
    return 0;
}

int statement() {
    int r;
    if (equal(sym.content, ";")) {
        print_word(sym);
        getsym();
    }
    else if (conditional_statement() >= 0) {}
    else if (loop_statement() >= 0) {}
    else if (equal(sym.content, "{")) {
        print_word(sym);
        getsym();
        check(statement_column())
        if (!equal(sym.content, "}")) {
            handle_error("no '}' after '{'");
            return -1;
        }
        print_word(sym);
        getsym();
    }
    else {
        if (func_call_statement() >= 0) {}
        else if (assignment_statement() >= 0) {}
        else if (read_statement() >= 0) {}
        else if (write_statement() >= 0) {}
        else if (return_statement() >= 0) {}
        else {
            return -1;
        }
        if (!equal(sym.content, ";")) {
            handle_error("no ';' after statement");
            return -1;
        }
        print_word(sym);
        getsym();
    }
    //printf("<语句>\n");
    return 0;
}

int conditional_statement() {
    int r;
    if (!equal(sym.content, "if")) {
        return -1;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, "(")) {
        handle_error("no '(' after if");
        return -1;
    }
    print_word(sym);
    getsym();
    check(condition())
    if (!equal(sym.content, ")")) {
        handle_error("no ')' after '('");
        return -1;
    }
    print_word(sym);
    getsym();
    check(statement())
    if (equal(sym.content, "else")) {
        print_word(sym);
        getsym();
        check(statement())
    }
    //printf("<条件语句>\n");
    return 0;
}

int condition() {
    int r;
    check(expression())
    if (sym.type >= 21 && sym.type <= 26) {
        print_word(sym);
        getsym();
        check(expression())
    }
    //printf("<条件>\n");
    return 0;
}

int expression() {
    int r;
    if ((equal(sym.content, "+") || equal(sym.content, "-")) && sym.type != 2) {
        print_word(sym);
        getsym();
    }
    check(term())
    while ((equal(sym.content, "+") || equal(sym.content, "-")) && sym.type != 2) {
        print_word(sym);
        getsym();
        check(term())
    }
    //printf("<表达式>\n");
    return 0;
}

int term() {
    int r;
    check(factor())
    while ((equal(sym.content, "*") || equal(sym.content, "/")) && sym.type != 2) {
        print_word(sym);
        getsym();
        check(factor())
    }
    //printf("<项>\n");
    return 0;
}

int factor() {
    int r;
    if (func_call_statement() >= 0) {}
    else if (equal(mark[sym.type], "IDENFR")) {
        print_word(sym);
        getsym();
        if (equal(sym.content, "[")) {
            print_word(sym);
            getsym();
            check(expression())
            if (!equal(sym.content, "]")) {
                handle_error("no ']' after '['");
                return -1;
            }
            print_word(sym);
            getsym();
        }
    }
    else if (equal(sym.content, "(")) {
        print_word(sym);
        getsym();
        check(expression())
        if (!equal(sym.content, ")")) {
            handle_error("no ')' after ')'");
            return -1;
        }
        print_word(sym);
        getsym();
    }
    else if (integer() >= 0) {}
    else if (equal(mark[sym.type], "CHARCON")) {
        print_word(sym);
        getsym();
    }
    else {
        //print_word(sym);
        handle_error("no matched factor");
        return -1;
    }
    //printf("<因子>\n");
    return 0;
}

int func_call_statement() {
    int r;
    char func[1000];
    if (!equal(mark[sym.type], "IDENFR")) {
        return -1;
    }
    strcpy(func, sym.content);
    if (!find_return_func(func) && !find_no_return_func(func)) {
        return -1;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, "(")) {
        handle_error("no '(' after func_call");
        return -1;
    }
    print_word(sym);
    getsym();
    check(value_parameter_table())
    if (!equal(sym.content, ")")) {
        handle_error("no ')' after '('");
        return -1;
    }
    print_word(sym);
    getsym();
    if (find_return_func(func)) {
        //printf("<有返回值函数调用语句>\n");
    }
    else if (find_no_return_func(func)) {
        //printf("<无返回值函数调用语句>\n");
    }
    else {
        handle_error("no such func in func_call");
        return -1;
    }
    return 0;
}

int value_parameter_table() {
    int r;
    if (equal(sym.content, ")")) {
        //printf("<值参数表>\n");
        return 0;
    }
    check(expression())
    while (equal(sym.content, ",")) {
        print_word(sym);
        getsym();
        check(expression())
    }
    //printf("<值参数表>\n");
    return 0;
}

int loop_statement() {
    int r;
    if (equal(sym.content, "while")) {
        print_word(sym);
        getsym();
        if (!equal(sym.content, "(")) {
            handle_error("no '(' after while");
            return -1;
        }
        print_word(sym);
        getsym();
        check(condition())
        if (!equal(sym.content, ")")) {
            handle_error("no ')' after '(' in while");
            return -1;
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
            return -1;
        }
        print_word(sym);
        getsym();
        if (!equal(sym.content, "(")) {
            handle_error("no '(' after do...while");
            return -1;
        }
        print_word(sym);
        getsym();
        check(condition())
        if (!equal(sym.content, ")")) {
            handle_error("no ')' after '(' in do...while");
            return -1;
        }
        print_word(sym);
        getsym();
    }
    else if (equal(sym.content, "for")) {
        print_word(sym);
        getsym();
        if (!equal(sym.content, "(")) {
            handle_error("no '(' after for");
            return -1;
        }
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return -1;
        }
        print_word(sym);
        getsym();
        if (!equal(sym.content, "=")) {
            handle_error("no '=' after for");
            return -1;
        }
        print_word(sym);
        getsym();
        check(expression())
        if (!equal(sym.content, ";")) {
            handle_error("no ';' after for");
            return -1;
        }
        print_word(sym);
        getsym();
        check(condition())
        if (!equal(sym.content, ";")) {
            handle_error("no ';' after for(;");
            return -1;
        }
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return -1;
        }
        print_word(sym);
        getsym();
        if (!equal(sym.content, "=")) {
            handle_error("no '=' after for(;;");
            return -1;
        }
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return -1;
        }
        print_word(sym);
        getsym();
        if (!equal(sym.content, "+") && !equal(sym.content, "-")) {
            handle_error("no '+'/'-' after for(;;");
            return -1;
        }
        print_word(sym);
        getsym();
        check(step())
        if (!equal(sym.content, ")")) {
            handle_error("no ')' after '(' in for");
            return -1;
        }
        print_word(sym);
        getsym();
        check(statement())
    }
    else {
        return -1;
    }
    //printf("<循环语句>\n");
    return 0;
}

int step() {
    int r;
    check(unsigned_integer())
    //printf("<步长>\n");
    return 0;
}

int assignment_statement() {
    int r;
    if (!equal(mark[sym.type], "IDENFR")) {
        return -1;
    }
    print_word(sym);
    getsym();
    if (equal(sym.content, "[")) {
        print_word(sym);
        getsym();
        check(expression())
        if (!equal(sym.content, "]")) {
            handle_error("no ']' after '[' in assignment_statement");
            return -1;
        }
        print_word(sym);
        getsym();
    }
    if (!equal(sym.content, "=")) {
        handle_error("no '=' or '[' in assignment_statement");
        return -1;
    }
    print_word(sym);
    getsym();
    check(expression())
    //printf("<赋值语句>\n");
    return 0;
}

int read_statement() {
    if (!equal(sym.content, "scanf")) {
        return -1;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, "(")) {
        handle_error("no '(' after scanf");
        return -1;
    }
    print_word(sym);
    getsym();
    if (!equal(mark[sym.type], "IDENFR")) {
        handle_error("wrong identifier!");
        return -1;
    }
    print_word(sym);
    getsym();
    while (equal(sym.content, ",")) {
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            handle_error("wrong identifier!");
            return -1;
        }
        print_word(sym);
        getsym();
    }
    if (!equal(sym.content, ")")) {
        handle_error("no ')' after '(' in scanf");
        return -1;
    }
    print_word(sym);
    getsym();
    //printf("<读语句>\n");
    return 0;
}

int write_statement() {
    int r;
    if (!equal(sym.content, "printf")) {
        return -1;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, "(")) {
        handle_error("no '(' after printf");
        return -1;
    }
    print_word(sym);
    getsym();
    if (string() >= 0) {
        if (equal(sym.content, ",")) {
            print_word(sym);
            getsym();
            check(expression())
        }
    }
    else if (expression() >= 0) {}
    else {
        handle_error("printf content error");
        return -1;
    }
    if (!equal(sym.content, ")")) {
        handle_error("no ')' after '(' in printf");
        return -1;
    }
    print_word(sym);
    getsym();
    //printf("<写语句>\n");
    return 0;
}

int string() {
    if (!equal(mark[sym.type], "STRCON")) {
        return -1;
    }
    print_word(sym);
    getsym();
    //printf("<字符串>\n");
    return 0;
}

int return_statement() {
    int r;
    if (!equal(sym.content, "return")) {
        return -1;
    }
    print_word(sym);
    getsym();
    if (equal(sym.content, "(")) {
        print_word(sym);
        getsym();
        check(expression())
        if (!equal(sym.content, ")")) {
            handle_error("no ')' after '(' in return_statement");
            return -1;
        }
        print_word(sym);
        getsym();
    }
    //printf("<返回语句>\n");
    return 0;
}

int no_return_func_definition() {
    int r;
    if (!equal(sym.content, "void")) {
        handle_error("no 'void' in no_return_func_definition");
        return -1;
    }
    strcpy(pre_read1.content, sym.content);
    pre_read1.type = sym.type;
    var1 = 1;
    getsym();
    if (equal(sym.content, "main")) {
        return -1;
    }
    print_word(pre_read1);
    var1 = 0;
    strcpy(no_return_func[no_return_func_index++], sym.content);
    print_word(sym);
    getsym();
    if (!equal(sym.content, "(")) {
        handle_error("no '(' after func header");
        return -1;
    }
    print_word(sym);
    getsym();
    check(parameter_table())
    if (!equal(sym.content, ")")) {
        handle_error("no ')' after '('");
        return -1;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, "{")) {
        handle_error("no '{' after func header");
        return -1;
    }
    print_word(sym);
    getsym();
    check(compound_statement())
    if (!equal(sym.content, "}")) {
        handle_error("no '}' after '{'");
        return -1;
    }
    print_word(sym);
    getsym();
    //printf("<无返回值函数定义>\n");
    return 0;
}

int find_no_return_func(char *func_to_find) {
    int r;
    for (r = 0; r < no_return_func_index; r++) {
        if (strcmp(no_return_func[r], func_to_find) == 0) {
            return 1;
        }
    }
    return 0;
}

int main_func() {
    int r;
    if ((var1 == 1 && !equal(pre_read1.content, "void")) || (var1 == 0 && !equal(sym.content, "void"))) {
        handle_error("no 'void' in main_func");
        return -1;
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
        return -1;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, "(")) {
        handle_error("no '(' after func header");
        return -1;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, ")")) {
        handle_error("no ')' after '('");
        return -1;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, "{")) {
        handle_error("no '{' after func header");
        return -1;
    }
    print_word(sym);
    getsym();
    check(compound_statement())
    if (!equal(sym.content, "}")) {
        handle_error("no '}' after '{'");
        return -1;
    }
    print_word(sym);
    //printf("<主函数>\n");
    return 0;
}