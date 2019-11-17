#ifndef GENERATE_GRAMMAR_H
#define GENERATE_GRAMMAR_H

typedef struct parser {
    int ret;
    char content[6][100];
} Parser;

static Parser zero = {0}, negative = {-1};

int parse();

void parser_print(char *string);

Parser program();

Parser constant_specification();

Parser constant_definition();

Parser integer();

Parser unsigned_integer();

Parser variable_specification();

Parser variable_definition();

Parser return_func_definition();

int find_return_func(char *func_to_find);

Parser declaration_header();

Parser parameter_table();

Parser compound_statement();

Parser statement_column();

Parser statement();

Parser conditional_statement();

Parser condition();

Parser expression();

Parser term();

Parser factor();

Parser func_call_statement();

Parser value_parameter_table();

Parser loop_statement();

Parser step();

Parser assignment_statement();

Parser read_statement();

Parser write_statement();

Parser string();

Parser return_statement();

Parser no_return_func_definition();

int find_no_return_func(char *func_to_find);

Parser main_func();

#endif //GENERATE_GRAMMAR_H
