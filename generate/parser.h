#ifndef GENERATE_GRAMMAR_H
#define GENERATE_GRAMMAR_H

typedef struct {
    int ret;
    int c_len;
    int kind; //0:int; 1:char;
    char content[6][100];
} Parser;

static Parser zero = {0}, negative = {-1};

typedef struct {
    char name[100];
    int kind; //0:int; 1:char;
    int usage; //0:name; 1:replace;
    char replace[100];
    int replace_kind; //0:int; 1:char;
} Var_list;

int parse();

void parser_print(char *string);

Parser program();

Parser constant_specification();

Parser constant_definition();

int find_var(char *name);

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
