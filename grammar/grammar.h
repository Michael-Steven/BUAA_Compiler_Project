#ifndef GRAMMAR_GRAMMAR_H
#define GRAMMAR_GRAMMAR_H

int program();

int constant_specification();

int constant_definition();

int integer();

int unsigned_integer();

int variable_specification();

int variable_definition();

int return_func_definition();

int find_return_func(char *func_to_find);

int declaration_header();

int parameter_table();

int compound_statement();

int statement_column();

int statement();

int conditional_statement();

int condition();

int expression();

int term();

int factor();

int func_call_statement();

int value_parameter_table();

int loop_statement();

int step();

int assignment_statement();

int read_statement();

int write_statement();

int string();

int return_statement();

int no_return_func_definition();

int find_no_return_func(char *func_to_find);

int main_func();

#endif //GRAMMAR_GRAMMAR_H
