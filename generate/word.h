#ifndef GENERATE_WORD_H
#define GENERATE_WORD_H

typedef struct Element {
    int type;
    char content[1000];
} Element;

int test_word();

int getsym();

int gettoken();

void print_word(Element single_word);

void handle_error(char err[]);

int handle_IDENFR();

int handle_INTCON();

int handle_CHARCON();

int handle_STRCON();

int handle_less_than();

int handle_greater_than();

int handle_equal();

#endif //GENERATE_WORD_H
