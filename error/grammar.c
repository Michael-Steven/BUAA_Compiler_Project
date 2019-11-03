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
        printf("compile error\n");
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
            handle_error("no ';' behind const definition", 'k');
        }
        else {
            print_word(sym);
            getsym();
        }
    }
    //printf("<常量说明>\n");
    return 0;
}

int constant_definition() {
    if (equal(sym.content, "int")) {
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            //handle_error("wrong identifier!",'a');
            return -1;
        }
        if (check_name_list(sym.content, deep)) {
            handle_error("redefined identifier", 'b');
        }
        else {
            add_name_list(sym.content, deep, 0);
        }
        print_word(sym);
        getsym();
        if (!equal(sym.content, "=")) {
            //handle_error("no '=' behind const int");
            return -1;
        }
        print_word(sym);
        getsym();
        if (integer() < 0) {
            handle_error("not integer", 'o');
            getsym();
        }
        while (equal(sym.content, ",")) {
            print_word(sym);
            getsym();
            if (!equal(mark[sym.type], "IDENFR")) {
                //handle_error("wrong identifier!");
                return -1;
            }
            if (check_name_list(sym.content, deep)) {
                handle_error("redefined identifier", 'b');
            }
            else {
                add_name_list(sym.content, deep, 0);
            }
            print_word(sym);
            getsym();
            if (!equal(sym.content, "=")) {
                //handle_error("no '=' behind const int, int");
                return -1;
            }
            print_word(sym);
            getsym();
            if (integer() < 0) {
                handle_error("not integer", 'o');
                getsym();
            }
        }
        //printf("<常量定义>\n");
        return 0;
    }
    if (equal(sym.content, "char")) {
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            //handle_error("wrong identifier!");
            return -1;
        }
        if (check_name_list(sym.content, deep)) {
            handle_error("redefined identifier", 'b');
        }
        else {
            add_name_list(sym.content, deep, 1);
        }
        print_word(sym);
        getsym();
        if (!equal(sym.content, "=")) {
            //handle_error("no '=' behind const char");
            return -1;
        }
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "CHARCON")) {
            handle_error("wrong character", 'o');
            getsym();
        }
        print_word(sym);
        getsym();
        while (equal(sym.content, ",")) {
            print_word(sym);
            getsym();
            if (!equal(mark[sym.type], "IDENFR")) {
                //handle_error("wrong identifier!");
                return -1;
            }
            if (check_name_list(sym.content, deep)) {
                handle_error("redefined identifier", 'b');
            }
            else {
                add_name_list(sym.content, deep, 1);
            }
            print_word(sym);
            getsym();
            if (!equal(sym.content, "=")) {
                //handle_error("no '=' behind const char, char");
                return -1;
            }
            print_word(sym);
            getsym();
            if (!equal(mark[sym.type], "CHARCON")) {
                handle_error("wrong character", 'o');
                getsym();
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
            handle_error("no ';' behind const definition", 'k');
        }
        else {
            print_word(sym);
            getsym();
        }
    }
    if (cnt == 0) {
        return 0;
    }
    //printf("<变量说明>\n");
    return 0;
}

int variable_definition() {
    int t;
    if (equal(pre_read1.content, "int")) {
        t = 2;
    }
    else {
        t = 3;
    }
    print_word(pre_read1);
    var1 = 0;
    if (!equal(mark[pre_read2.type], "IDENFR")) {
        //handle_error("wrong identifier!");
        return -1;
    }
    if (check_name_list(pre_read2.content, deep)) {
        handle_error("redefined identifier", 'b');
    }
    else {
        add_name_list(pre_read2.content, deep, t);
    }
    print_word(pre_read2);
    var2 = 0;
    if (equal(sym.content, "[")) {
        print_word(sym);
        getsym();
        if (unsigned_integer() == 0) {
            //handle_error("number of array is 0");
            return -1;
        }
        if (!equal(sym.content, "]")) {
            handle_error("no ']' behind const definition", 'm');
        }
        else {
            print_word(sym);
            getsym();
        }
    }
    while (equal(sym.content, ",")) {
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            //handle_error("wrong identifier!");
            return -1;
        }
        if (check_name_list(sym.content, deep)) {
            handle_error("redefined identifier", 'b');
        }
        else {
            add_name_list(sym.content, deep, t);
        }
        print_word(sym);
        getsym();
        if (equal(sym.content, "[")) {
            print_word(sym);
            getsym();
            if (unsigned_integer() == 0) {
                //handle_error("number of array is 0");
                return -1;
            }
            if (!equal(sym.content, "]")) {
                handle_error("no ']' behind const definition", 'm');
            }
            else {
                print_word(sym);
                getsym();
            }
        }
    }
    //printf("<变量定义>\n");
    return 0;
}

int return_func_definition() {
    int r, type;
    check(declaration_header())
    type = r;
    deep++;
    if (!equal(sym.content, "(")) {
        //handle_error("no '(' after func header");
        return -1;
    }
    print_word(sym);
    getsym();
    check(parameter_table(0))
    return_func_index++;
    if (!equal(sym.content, ")")) {
        handle_error("no ')' behind const definition", 'l');
    }
    else {
        print_word(sym);
        getsym();
    }
    if (!equal(sym.content, "{")) {
        //handle_error("no '{' after func header");
        return -1;
    }
    print_word(sym);
    getsym();
    if (type == 0) {
        check(compound_statement(1))
    }
    else {
        check(compound_statement(2))
    }
    if (!equal(sym.content, "}")) {
        //handle_error("no '}' after '{'");
        return -1;
    }
    delete_name_list(deep);
    deep--;
    print_word(sym);
    getsym();
    //printf("<有返回值函数定义>\n");
    return 0;
}

int find_return_func(char *func_to_find) {
    int r;
    for (r = 0; r < return_func_index; r++) {
        if (strcmp(return_func[r].content, func_to_find) == 0) {
            return 1;
        }
    }
    return 0;
}

FUNC find_func(char *func_to_find) {
    int r;
    FUNC tmp;
    tmp.type = 3;
    for (r = 0; r < return_func_index; r++) {
        if (strcmp(return_func[r].content, func_to_find) == 0) {
            return return_func[r];
        }
    }
    for (r = 0; r < no_return_func_index; r++) {
        if (strcmp(no_return_func[r].content, func_to_find) == 0) {
            return no_return_func[r];
        }
    }
    return tmp;
}

int declaration_header() {
    int type;
    if ((var1 == 1 && !equal(pre_read1.content, "int") && !equal(pre_read1.content, "char"))
        || (var1 == 0 && !equal(sym.content, "int") && !equal(sym.content, "char"))) {
        return -1;
    }
    if (var1 == 1) {
        var1 = 0;
        print_word(pre_read1);
        if (equal(pre_read1.content, "int")) {
            type = 0;
            return_func[return_func_index].type = 0;
        }
        else {
            type = 1;
            return_func[return_func_index].type = 1;
        }
    }
    else {
        if (equal(sym.content, "int")) {
            type = 0;
            return_func[return_func_index].type = 0;
        }
        else {
            type = 1;
            return_func[return_func_index].type = 1;
        }
        print_word(sym);
        getsym();
    }
    if ((var2 == 1 && !equal(mark[pre_read2.type], "IDENFR"))
        || (var2 == 0 && !equal(mark[sym.type], "IDENFR"))) {
        //handle_error("wrong identifier!");
        return -1;
    }
    if (var2 == 1) {
        var2 = 0;
        strcpy(return_func[return_func_index].content, pre_read2.content);
        if (check_name_list(pre_read2.content, deep)) {
            handle_error("redefined identifier", 'b');
        }
        else {
            add_name_list(pre_read2.content, deep, 4);
        }
        print_word(pre_read2);
    }
    else {
        strcpy(return_func[return_func_index].content, sym.content);
        if (check_name_list(sym.content, deep)) {
            handle_error("redefined identifier", 'b');
        }
        else {
            add_name_list(sym.content, deep, 4);
        }
        print_word(sym);
        getsym();
    }
    //printf("<声明头部>\n");
    return type;
}

int parameter_table(int t) {
    if (equal(sym.content, ")")) {
        //printf("<参数表>\n");
        return 0;
    }
    return_func[return_func_index].para_sum = 0;
    if (!equal(sym.content, "int") && !equal(sym.content, "char")) {
        //handle_error("no int/char in parameter_table");
        return -1;
    }
    int type;
    if (t == 0) {
        if (equal(sym.content, "int")) {
            type = 2;
            return_func[return_func_index].para_type[return_func[return_func_index].para_sum++] = 0;
        }
        else {
            type = 3;
            return_func[return_func_index].para_type[return_func[return_func_index].para_sum++] = 1;
        }
    }
    else {
        if (equal(sym.content, "int")) {
            type = 2;
            no_return_func[no_return_func_index].para_type[no_return_func[no_return_func_index].para_sum++] = 0;
        }
        else {
            type = 3;
            no_return_func[no_return_func_index].para_type[no_return_func[no_return_func_index].para_sum++] = 1;
        }
    }
    print_word(sym);
    getsym();
    if (!equal(mark[sym.type], "IDENFR")) {
        //handle_error("wrong identifier!");
        return -1;
    }
    if (check_name_list(sym.content, deep)) {
        handle_error("redefined identifier", 'b');
    }
    else {
        add_name_list(sym.content, deep, type);
    }
    print_word(sym);
    getsym();
    while (equal(sym.content, ",")) {
        print_word(sym);
        getsym();
        if (!equal(sym.content, "int") && !equal(sym.content, "char")) {
            //handle_error("no int/char after ','");
            return -1;
        }
        if (t == 0) {
            if (equal(sym.content, "int")) {
                type = 2;
                return_func[return_func_index].para_type[return_func[return_func_index].para_sum++] = 0;
            }
            else {
                type = 3;
                return_func[return_func_index].para_type[return_func[return_func_index].para_sum++] = 1;
            }
        }
        else {
            if (equal(sym.content, "int")) {
                type = 2;
                no_return_func[no_return_func_index].para_type[no_return_func[no_return_func_index].para_sum++] = 0;
            }
            else {
                type = 3;
                no_return_func[no_return_func_index].para_type[no_return_func[no_return_func_index].para_sum++] = 1;
            }
        }
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            //handle_error("wrong identifier!");
            return -1;
        }
        if (check_name_list(sym.content, deep)) {
            handle_error("redefined identifier", 'b');
        }
        else {
            add_name_list(sym.content, deep, type);
        }
        print_word(sym);
        getsym();
    }
    //printf("<参数表>\n");
    return 0;
}

int compound_statement(int t) {
    int r;
    check(constant_specification())
    check(variable_specification())
    check(statement_column(t))
    if ((t == 1 || t == 2) && r == 0) {
        handle_error("Function with return value is missing return statement", 'h');
    }
    //printf("<复合语句>\n");
    return 0;
}

int statement_column(int t) {
    int r, flag = 0;
    while ((r = statement(t)) >= 0) {
        if (r == 1) {
            flag = 1;
        }
    }
    if (flag == 1) {
        return 1;
    }
    //printf("<语句列>\n");
    return 0;
}

int statement(int t) {
    int r, flag = 0;
    if (equal(sym.content, ";")) {
        print_word(sym);
        getsym();
    }
    else if ((r = conditional_statement(t)) >= 0) {
        if (r == 1) {
            flag = 1;
        }
    }
    else if ((r = loop_statement(t)) >= 0) {
        if (r == 1) {
            flag = 1;
        }
    }
    else if (equal(sym.content, "{")) {
        print_word(sym);
        getsym();
        check(statement_column(t))
        if (r == 1) {
            flag = 1;
        }
        if (!equal(sym.content, "}")) {
            //handle_error("no '}' after '{'");
            return -1;
        }
        print_word(sym);
        getsym();
    }
    else {
        if (read_statement() >= 0) {}
        else if (write_statement() >= 0) {}
        else if (return_statement(t) >= 0) {
            flag = 1;
        }
        else if (assignment_statement() >= 0) {}
        else if (func_call_statement() >= 0) {}
        else {
            return -1;
        }
        if (!equal(sym.content, ";")) {
            handle_error("no ';' behind const definition", 'k');
        }
        else {
            print_word(sym);
            getsym();
        }
    }
    if (flag == 1) {
        return 1;
    }
    //printf("<语句>\n");
    return 0;
}

int conditional_statement(int t) {
    int r, flag = 0;
    if (!equal(sym.content, "if")) {
        return -1;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, "(")) {
        //handle_error("no '(' after if");
        return -1;
    }
    print_word(sym);
    getsym();
    check(condition())
    if (!equal(sym.content, ")")) {
        handle_error("no ')' behind const definition", 'l');
    }
    else {
        print_word(sym);
        getsym();
    }
    check(statement(t))
    if (r == 1) {
        flag = 1;
    }
    if (equal(sym.content, "else")) {
        print_word(sym);
        getsym();
        check(statement(t))
        if (r == 1) {
            flag = 1;
        }
    }
    //printf("<条件语句>\n");
    if (flag == 1) {
        return 1;
    }
    return 0;
}

int condition() {
    int r;
    check(expression())
    if (r != 0) {
        handle_error("Illegal type in condition judgment", 'f');
    }
    if (sym.type >= 21 && sym.type <= 26) {
        print_word(sym);
        getsym();
        check(expression())
        if (r != 0) {
            handle_error("Illegal type in condition judgment", 'f');
        }
    }
    //printf("<条件>\n");
    return 0;
}

int expression() {
    int r, flag = 0;
    if ((equal(sym.content, "+") || equal(sym.content, "-")) && sym.type != 2) {
        flag = 1;
        print_word(sym);
        getsym();
    }
    check(term())
    while ((equal(sym.content, "+") || equal(sym.content, "-")) && sym.type != 2) {
        flag = 1;
        print_word(sym);
        getsym();
        check(term())
    }
    //printf("<表达式>\n");
    if (flag == 1) {
        return 0;
    }
    else {
        return r;
    }
}

int term() {
    int r, flag = 0;
    check(factor())
    while ((equal(sym.content, "*") || equal(sym.content, "/")) && sym.type != 2) {
        flag = 1;
        print_word(sym);
        getsym();
        check(factor())
    }
    //printf("<项>\n");
    if (flag == 1) {
        return 0;
    }
    else {
        return r;
    }
}

int factor() {
    int r;
    if (equal(sym.content, "(")) {
        print_word(sym);
        getsym();
        check(expression())
        if (!equal(sym.content, ")")) {
            handle_error("no ')' behind const definition", 'l');
        }
        else {
            print_word(sym);
            getsym();
        }
        return 0;
    }
    else if (integer() >= 0) {
        return 0;
    }
    else if (equal(mark[sym.type], "CHARCON")) {
        print_word(sym);
        getsym();
        return 1;
    }
    else if (equal(mark[sym.type], "IDENFR")) {
        int location = search_name_list(sym.content, 0);
        if (location == 0) {
            handle_error("not defined identifier", 'c');
        }
        strcpy(pre_read1.content, sym.content);
        pre_read1.type = sym.type;
        var1 = 1;
        getsym();
//        print_word(sym);
//        getsym();
        if (equal(sym.content, "(")) {
            if ((r = func_call_statement()) == 0) {
                return 0;
            }
            else {
                return 1;
            }
        }
        if (equal(sym.content, "[")) {
            print_word(pre_read1);
            var1 = 0;
            print_word(sym);
            getsym();
            check(expression())
            if (r != 0) {
                handle_error("Subscript of array element is not an integer expression", 'i');
            }
            if (!equal(sym.content, "]")) {
                handle_error("no ']' behind const definition", 'm');
            }
            else {
                print_word(sym);
                getsym();
            }
        }
        if (var1 == 1) {
            print_word(pre_read1);
            var1 = 0;
        }
        if (name_list[location].type == 1 || name_list[location].type == 3) {
            return 1;
        }
        else {
            return 0;
        }
    }
        //else if (func_call_statement() >= 0) {}
    else {
        //print_word(sym);
        //handle_error("no matched factor");
        return -1;
    }
    //printf("<因子>\n");
}

int func_call_statement() {
    int r;
    char func[1000];
    FUNC f = find_func(func);
    if ((var1 == 0 && !equal(mark[sym.type], "IDENFR")) || (var1 == 1 && !equal(mark[pre_read1.type], "IDENFR"))) {
        return -1;
    }
    if (var1 == 0) {
        print_word(sym);
        getsym();
        strcpy(func, sym.content);
    }
    else if (var1 == 1) {
        var1 = 0;
        print_word(pre_read1);
        strcpy(func, pre_read1.content);
    }
//    if (!find_return_func(func) && !find_no_return_func(func)) {
//        return -1;
//    }
    if (search_name_list(func, 4) < 0) {
        handle_error("not defined identifier", 'c');
    }
    if (!equal(sym.content, "(")) {
        //handle_error("no '(' after func_call");
        return -1;
    }
    print_word(sym);
    getsym();
    check(value_parameter_table(func))
    if (!equal(sym.content, ")")) {
        handle_error("no ')' behind const definition", 'l');
    }
    else {
        print_word(sym);
        getsym();
    }
//    if (find_return_func(func)) {
//        //printf("<有返回值函数调用语句>\n");
//    }
//    else if (find_no_return_func(func)) {
//        //printf("<无返回值函数调用语句>\n");
//    }
//    else {
//        //handle_error("no such func in func_call");
//        return -1;
//    }
    if (f.type == 0) {
        return 0;
    }
    else {
        return 1;
    }
}

int value_parameter_table(char *func) {
    int r, cnt = 0;
    FUNC f = find_func(func);
    if (equal(sym.content, ")")) {
        //printf("<值参数表>\n");
        return 0;
    }
    check(expression())
    if (r != f.para_type[cnt]) {
        handle_error("Parameter type mismatch", 'e');
    }
    cnt++;
    while (equal(sym.content, ",")) {
        print_word(sym);
        getsym();
        check(expression())
        if (r != f.para_type[cnt]) {
            handle_error("Parameter type mismatch", 'e');
        }
        cnt++;
    }
    if (cnt != f.para_sum) {
        handle_error("Number of parameters does not match", 'd');
    }
    //printf("<值参数表>\n");
    return 0;
}

int loop_statement(int t) {
    int r, flag = 0;
    NAME_LIST name;
    if (equal(sym.content, "while")) {
        print_word(sym);
        getsym();
        if (!equal(sym.content, "(")) {
            //handle_error("no '(' after while");
            return -1;
        }
        print_word(sym);
        getsym();
        check(condition())
        if (!equal(sym.content, ")")) {
            handle_error("no ')' behind const definition", 'l');
        }
        else {
            print_word(sym);
            getsym();
        }
        check(statement(t))
        if (r == 1) {
            flag = 1;
        }
    }
    else if (equal(sym.content, "do")) {
        print_word(sym);
        getsym();
        check(statement(t))
        if (r == 1) {
            flag = 1;
        }
        if (!equal(sym.content, "while")) {
            handle_error("no while after do", 'n');
        }
        else {
            print_word(sym);
            getsym();
        }
        if (!equal(sym.content, "(")) {
            //handle_error("no '(' after do...while");
            return -1;
        }
        print_word(sym);
        getsym();
        check(condition())
        if (!equal(sym.content, ")")) {
            handle_error("no ')' behind const definition", 'l');
        }
        else {
            print_word(sym);
            getsym();
        }
    }
    else if (equal(sym.content, "for")) {
        print_word(sym);
        getsym();
        if (!equal(sym.content, "(")) {
            //handle_error("no '(' after for");
            return -1;
        }
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            //handle_error("wrong identifier!");
            return -1;
        }
        if (search_name_list(sym.content, 0) < 0) {
            handle_error("not defined identifier", 'c');
        }
        name = find_name_list(sym.content);
        if (name.type == 0 || name.type == 1) {
            handle_error("Constant value cannot be changed", 'j');
        }
        print_word(sym);
        getsym();
        if (!equal(sym.content, "=")) {
            //handle_error("no '=' after for");
            return -1;
        }
        print_word(sym);
        getsym();
        check(expression())
        if (!equal(sym.content, ";")) {
            handle_error("no ';' behind const definition", 'k');
        }
        else {
            print_word(sym);
            getsym();
        }
        check(condition())
        if (!equal(sym.content, ";")) {
            handle_error("no ';' behind const definition", 'k');
        }
        else {
            print_word(sym);
            getsym();
        }
        if (!equal(mark[sym.type], "IDENFR")) {
            //handle_error("wrong identifier!");
            return -1;
        }
        if (search_name_list(sym.content, 0) < 0) {
            handle_error("not defined identifier", 'c');
        }
        name = find_name_list(sym.content);
        if (name.type == 0 || name.type == 1) {
            handle_error("Constant value cannot be changed", 'j');
        }
        print_word(sym);
        getsym();
        if (!equal(sym.content, "=")) {
            //handle_error("no '=' after for(;;");
            return -1;
        }
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            //handle_error("wrong identifier!");
            return -1;
        }
        if (search_name_list(sym.content, 0) < 0) {
            handle_error("not defined identifier", 'c');
        }
        print_word(sym);
        getsym();
        if (!equal(sym.content, "+") && !equal(sym.content, "-")) {
            //handle_error("no '+'/'-' after for(;;");
            return -1;
        }
        print_word(sym);
        getsym();
        check(step())
        if (!equal(sym.content, ")")) {
            handle_error("no ')' behind const definition", 'l');
        }
        else {
            print_word(sym);
            getsym();
        }
        check(statement(t))
        if (r == 1) {
            flag = 1;
        }
    }
    else {
        return -1;
    }
    //printf("<循环语句>\n");
    if (flag == 1) {
        return 1;
    }
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
    NAME_LIST name;
    if (!equal(mark[sym.type], "IDENFR")) {
        return -1;
    }
    strcpy(pre_read1.content, sym.content);
    pre_read1.type = sym.type;
    var1 = 1;
    getsym();
//    print_word(sym);
//    getsym();
    if (equal(sym.content, "(")) {
        return -1;
    }
    if (search_name_list(pre_read1.content, 0) < 0) {
        handle_error("not defined identifier", 'c');
    }
    name = find_name_list(pre_read1.content);
    if (name.type == 0 || name.type == 1) {
        handle_error("Constant value cannot be changed", 'j');
    }
    if (equal(sym.content, "[")) {
        print_word(pre_read1);
        var1 = 0;
        print_word(sym);
        getsym();
        check(expression())
        if (r != 0) {
            handle_error("Subscript of array element is not an integer expression", 'i');
        }
        if (!equal(sym.content, "]")) {
            handle_error("no ']' behind const definition", 'm');
        }
        else {
            print_word(sym);
            getsym();
        }
    }
    if (var1 == 1) {
        var1 = 0;
        print_word(pre_read1);
    }
    if (!equal(sym.content, "=")) {
        //handle_error("no '=' or '[' in assignment_statement");
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
        //handle_error("no '(' after scanf");
        return -1;
    }
    print_word(sym);
    getsym();
    if (!equal(mark[sym.type], "IDENFR")) {
        //handle_error("wrong identifier!");
        return -1;
    }
    if (search_name_list(sym.content, 0) < 0) {
        handle_error("not defined identifier", 'c');
    }
    print_word(sym);
    getsym();
    while (equal(sym.content, ",")) {
        print_word(sym);
        getsym();
        if (!equal(mark[sym.type], "IDENFR")) {
            //handle_error("wrong identifier!");
            return -1;
        }
        if (search_name_list(sym.content, 0) < 0) {
            handle_error("not defined identifier", 'c');
        }
        print_word(sym);
        getsym();
    }
    if (!equal(sym.content, ")")) {
        handle_error("no ')' behind const definition", 'l');
    }
    else {
        print_word(sym);
        getsym();
    }
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
        //handle_error("no '(' after printf");
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
        //handle_error("printf content error");
        return -1;
    }
    if (!equal(sym.content, ")")) {
        handle_error("no ')' behind const definition", 'l');
    }
    else {
        print_word(sym);
        getsym();
    }
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

int return_statement(int t) {
    int r = 0, flag = 0;
    if (!equal(sym.content, "return")) {
        return -1;
    }
    print_word(sym);
    getsym();
    if (equal(sym.content, "(")) {
        flag = 1;
        print_word(sym);
        getsym();
        if ((r = expression()) < 0) {
            return r;
        }
        if (!equal(sym.content, ")")) {
            handle_error("no ')' behind const definition", 'l');
        }
        else {
            print_word(sym);
            getsym();
        }
    }
    //printf("<返回语句>\n");
    if (flag == 1 && t == 0) {
        handle_error("Mismatched return statement for function without return value", 'g');
    }
    else if ((t == 1 && (flag == 0 || r == 1)) || (t == 2 && (flag == 0 || r == 0))) {
        handle_error("Mismatched return statement for function with return value", 'h');
    }
    return 0;
}

int no_return_func_definition() {
    int r;
    if (!equal(sym.content, "void")) {
        //handle_error("no 'void' in no_return_func_definition");
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
    if (check_name_list(sym.content, deep)) {
        handle_error("redefined identifier", 'b');
    }
    else {
        add_name_list(sym.content, deep, 4);
    }
    no_return_func[no_return_func_index].type = 2;
    strcpy(no_return_func[no_return_func_index].content, sym.content);
    print_word(sym);
    getsym();
    deep++;
    if (!equal(sym.content, "(")) {
        //handle_error("no '(' after func header");
        return -1;
    }
    print_word(sym);
    getsym();
    check(parameter_table(1))
    no_return_func_index++;
    if (!equal(sym.content, ")")) {
        handle_error("no ')' behind const definition", 'l');
    }
    else {
        print_word(sym);
        getsym();
    }
    if (!equal(sym.content, "{")) {
        //handle_error("no '{' after func header");
        return -1;
    }
    print_word(sym);
    getsym();
    check(compound_statement(0))
    if (!equal(sym.content, "}")) {
        //handle_error("no '}' after '{'");
        return -1;
    }
    delete_name_list(deep);
    deep--;
    print_word(sym);
    getsym();
    //printf("<无返回值函数定义>\n");
    return 0;
}

int find_no_return_func(char *func_to_find) {
    int r;
    for (r = 0; r < no_return_func_index; r++) {
        if (strcmp(no_return_func[r].content, func_to_find) == 0) {
            return 1;
        }
    }
    return 0;
}

int main_func() {
    int r;
    if ((var1 == 1 && !equal(pre_read1.content, "void")) || (var1 == 0 && !equal(sym.content, "void"))) {
        //handle_error("no 'void' in main_func");
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
        //handle_error("no 'main' in main_func");
        return -1;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, "(")) {
        //handle_error("no '(' after func header");
        return -1;
    }
    print_word(sym);
    getsym();
    if (!equal(sym.content, ")")) {
        handle_error("no ')' behind const definition", 'l');
    }
    else {
        print_word(sym);
        getsym();
    }
    deep++;
    if (!equal(sym.content, "{")) {
        //handle_error("no '{' after func header");
        return -1;
    }
    print_word(sym);
    getsym();
    check(compound_statement(0))
    if (!equal(sym.content, "}")) {
        //handle_error("no '}' after '{'");
        return -1;
    }
    delete_name_list(deep);
    deep--;
    print_word(sym);
    //printf("<主函数>\n");
    return 0;
}

void add_name_list(char *a, int d, int t) {
    strcpy(name_list[name_list_index].content, a);
    name_list[name_list_index].deep = d;
    name_list[name_list_index].type = t;
    name_list_index++;
}

void delete_name_list(int d) {
    while (name_list[name_list_index - 1].deep == d) {
        name_list_index--;
    }
}

int search_name_list(char *a, int t) {
    int i;
    for (i = name_list_index - 1; i >= 0; i--) {
        if (strcmp(name_list[i].content, a) == 0) {
            if ((name_list[i].type == 4 && t == 4) || t == 3 || t == 2 || t == 1 || t == 0) {
                return i;
            }
            else {
                return -1;
            }
        }
    }
    return -1;
}

NAME_LIST find_name_list(char *a) {
    int i;
    NAME_LIST tmp;
    tmp.type = 5;
    for (i = name_list_index - 1; i >= 0; i--) {
        if (strcmp(name_list[i].content, a) == 0) {
            return name_list[i];
        }
    }
    return tmp;
}

int check_name_list(char *a, int d) {
    int i;
    for (i = name_list_index - 1; name_list[i].deep == d; i--) {
        if (strcmp(name_list[i].content, a) == 0) {
            return 1;
        }
    }
    return 0;
}