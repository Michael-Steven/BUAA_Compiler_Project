#ifndef WORD_WORD_H
#define WORD_WORD_H

void gettoken();
void handle_error(char err[]);
void handle_IDENFR();
void handle_INTCON();
void handle_CHARCON();
void handle_STRCON();
void handle_less_than();
void handle_greater_than();
void handle_equal();

typedef struct Element {
    int type;
    char content[1000];
} Element;

const char mark[40][10] = {"IDENFR", "INTCON", "CHARCON", "STRCON", "CONSTTK", "INTTK", "CHARTK",
                           "VOIDTK", "MAINTK", "IFTK", "ELSETK", "DOTK", "WHILETK", "FORTK",
                           "SCANFTK", "PRINTFTK", "RETURNTK", "PLUS", "MINU", "MULT", "DIV",
                           "LSS", "LEQ", "GRE", "GEQ", "EQL", "NEQ", "ASSIGN", "SEMICN",
                           "COMMA", "LPARENT", "RPARENT", "LBRACK", "RBRACK", "LBRACE", "RBRACE"};
Element word;
char c;
char s[1000];

#endif //WORD_WORD_H
