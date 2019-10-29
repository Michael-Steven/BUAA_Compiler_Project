#ifndef GRAMMAR_WORD_H
#define GRAMMAR_WORD_H

#define equal(A, B) (strcmp(A, B) == 0)

typedef struct Element {
    int type;
    char content[1000];
} Element;


static char mark[40][10] = {"IDENFR", "INTCON", "CHARCON", "STRCON", "CONSTTK", "INTTK", "CHARTK",
                            "VOIDTK", "MAINTK", "IFTK", "ELSETK", "DOTK", "WHILETK", "FORTK",
                            "SCANFTK", "PRINTFTK", "RETURNTK", "PLUS", "MINU", "MULT", "DIV",
                            "LSS", "LEQ", "GRE", "GEQ", "EQL", "NEQ", "ASSIGN", "SEMICN",
                            "COMMA", "LPARENT", "RPARENT", "LBRACK", "RBRACK", "LBRACE", "RBRACE"};

Element sym;    // "getsym()" save a word struct in sym
Element pre_read1, pre_read2, pre_read3;
static int var1 = 0, var2 = 0, var3 = 0;
char c;         // used to getchar()
char s[1000];   // temp var to save a string
static char return_func[100][1000] = {};
static char no_return_func[100][1000] = {};
static int return_func_index = 0;
static int no_return_func_index = 0;

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

#endif //GRAMMAR_WORD_H
