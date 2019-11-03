#ifndef GRAMMAR_GRAMMAR_H
#define GRAMMAR_GRAMMAR_H

typedef struct name_list {
    char content[100];
    int deep;
    int type; //0:const_int; 1:const_char; 2:var_int; 3:var_char; 4:func;
} NAME_LIST;

NAME_LIST name_list[1000];
static int name_list_index = 0;
static int deep = 0;

int program();

int constant_specification();

int constant_definition();

int integer();

int unsigned_integer();

int variable_specification();

int variable_definition();

int return_func_definition();

int find_return_func(char *func_to_find);

FUNC find_func(char *func_to_find);

int declaration_header(); // 0:int; 1:char;

int parameter_table(int t); // 0:return; 1:no_return;

int compound_statement(int t); // 0:no_return; 1:return_int; 2:return_char;

int statement_column(int t);

int statement(int t); // 0:not_have_return; 1:have_return;

int conditional_statement(int t);

int condition();

int expression(); // 0:int; 1:char;

int term();

int factor();

int func_call_statement();

int value_parameter_table(char *func);

int loop_statement(int t);

int step();

int assignment_statement();

int read_statement();

int write_statement();

int string();

int return_statement(int t);

int no_return_func_definition();

int find_no_return_func(char *func_to_find);

int main_func();

void add_name_list(char *a, int d, int t);

void delete_name_list(int deep);

int search_name_list(char *a, int t);

NAME_LIST find_name_list(char *a);

int check_name_list(char *a, int d);

#endif //GRAMMAR_GRAMMAR_H
