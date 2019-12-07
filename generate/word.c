#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "word.h"
#include "error.h"
#include "file.h"

extern Element sym;
extern const char mark[40][10];
char c;         // used to getchar()
char s[1000];   // temp var to save a string

int test_word() {
//    freopen("testfile.txt", "r", stdin);
//    freopen("output.txt", "w", stdout);
    c = getchar();
    while (1) {
        while (isspace(c)) {
            c = getchar();
        }
        if (c == EOF) break;
        gettoken();
        fprintf(parser_out, "%s %s\n", mark[sym.type], sym.content);
    }
    return 0;
}

int getsym() {
    while (isspace(c)) {
        c = getchar();
    }
    if (c == EOF) {
        exit(0);
    }
    if (gettoken() < 0) {
        handle_error("getsym error\n");
        exit(0);
    }
    return 0;
}

int gettoken() {
    if (isalpha(c) || c == '_') {
        return handle_IDENFR();
    }
    else if (isdigit(c)) {
        return handle_INTCON();
    }
    else if (c == '\'') {
        return handle_CHARCON();
    }
    else if (c == '\"') {
        return handle_STRCON();
    }
    else if (c == '+') {
        sym.type = 17;
        strcpy(sym.content, "+");
        c = getchar();
    }
    else if (c == '-') {
        sym.type = 18;
        strcpy(sym.content, "-");
        c = getchar();
    }
    else if (c == '*') {
        sym.type = 19;
        strcpy(sym.content, "*");
        c = getchar();
    }
    else if (c == '/') {
        sym.type = 20;
        strcpy(sym.content, "/");
        c = getchar();
    }
    else if (c == '<') {
        c = getchar();
        handle_less_than();
    }
    else if (c == '>') {
        c = getchar();
        handle_greater_than();
    }
    else if (c == '=') {
        c = getchar();
        handle_equal();
    }
    else if (c == '!') {
        c = getchar();
        if (c == '=') {
            sym.type = 26;
            strcpy(sym.content, "!=");
            c = getchar();
        }
        else {
            handle_error("!= error");
        }
    }
    else if (c == ';') {
        sym.type = 28;
        strcpy(sym.content, ";");
        c = getchar();
    }
    else if (c == ',') {
        sym.type = 29;
        strcpy(sym.content, ",");
        c = getchar();
    }
    else if (c == '(') {
        sym.type = 30;
        strcpy(sym.content, "(");
        c = getchar();
    }
    else if (c == ')') {
        sym.type = 31;
        strcpy(sym.content, ")");
        c = getchar();
    }
    else if (c == '[') {
        sym.type = 32;
        strcpy(sym.content, "[");
        c = getchar();
    }
    else if (c == ']') {
        sym.type = 33;
        strcpy(sym.content, "]");
        c = getchar();
    }
    else if (c == '{') {
        sym.type = 34;
        strcpy(sym.content, "{");
        c = getchar();
    }
    else if (c == '}') {
        sym.type = 35;
        strcpy(sym.content, "}");
        c = getchar();
    }
    else {
        handle_error("unrecognized character");
        return -1;
    }
    return 0;
}

void print_word(Element single_word) {
#ifdef PARSER_OUT
    fprintf(parser_out, "%s %s\n", mark[single_word.type], single_word.content);
#endif
}

int handle_IDENFR() {
    int i = 0;
    s[i++] = c;
    while ((c = getchar())) {
        if (!isalnum(c) && c != '_') {
            break;
        }
        s[i++] = c;
    }
    s[i] = '\0';
    if (equal(s, "const")) {
        sym.type = 4;
        strcpy(sym.content, "const");
    }
    else if (equal(s, "int")) {
        sym.type = 5;
        strcpy(sym.content, "int");
    }
    else if (equal(s, "char")) {
        sym.type = 6;
        strcpy(sym.content, "char");
    }
    else if (equal(s, "void")) {
        sym.type = 7;
        strcpy(sym.content, "void");
    }
    else if (equal(s, "main")) {
        sym.type = 8;
        strcpy(sym.content, "main");
    }
    else if (equal(s, "if")) {
        sym.type = 9;
        strcpy(sym.content, "if");
    }
    else if (equal(s, "else")) {
        sym.type = 10;
        strcpy(sym.content, "else");
    }
    else if (equal(s, "do")) {
        sym.type = 11;
        strcpy(sym.content, "do");
    }
    else if (equal(s, "while")) {
        sym.type = 12;
        strcpy(sym.content, "while");
    }
    else if (equal(s, "for")) {
        sym.type = 13;
        strcpy(sym.content, "for");
    }
    else if (equal(s, "scanf")) {
        sym.type = 14;
        strcpy(sym.content, "scanf");
    }
    else if (equal(s, "printf")) {
        sym.type = 15;
        strcpy(sym.content, "printf");
    }
    else if (equal(s, "return")) {
        sym.type = 16;
        strcpy(sym.content, "return");
    }
    else {
        sym.type = 0;
        strcpy(sym.content, s);
    }
    return 0;
}

int handle_INTCON() {
    int i = 0;
    s[i++] = c;
    while ((c = getchar())) {
        if (!isdigit(c)) {
            break;
        }
        s[i++] = c;
    }
    s[i] = '\0';
    if (s[0] == '0' && isdigit(s[1])) {
        handle_error("0 at head of an unsigned integer");
        return -1;
    }
    sym.type = 1;
    strcpy(sym.content, s);
    return 0;
}

int handle_CHARCON() {
    int i = 0;
    c = getchar();
    if (c != '+' && c != '-' && c != '*' && c != '/' && c != '_' && !isalnum(c)) {
        handle_error("not a CHARCON");
        return -1;
    }
    s[i++] = c;
    s[i] = '\0';
    sym.type = 2;
    strcpy(sym.content, s);
    c = getchar();
    if (c != '\'') {
        handle_error("handle_CHARCON error");
        return -1;
    }
    c = getchar();
    return 0;
}

int handle_STRCON() {
    int i = 0;
    while ((c = getchar())) {
        if (c == '\"') {
            break;
        }
        s[i++] = c;
        if (c == '\\') {
            s[i++] = '\\';
        }
    }
    s[i] = '\0';
    sym.type = 3;
    strcpy(sym.content, s);
    if (c != '\"') {
        handle_error("handle_STRCON error");
        return -1;
    }
    c = getchar();
    return 0;
}

int handle_less_than() {
    if (c == '=') {
        sym.type = 22;
        strcpy(sym.content, "<=");
        c = getchar();
    }
    else {
        sym.type = 21;
        strcpy(sym.content, "<");
    }
    return 0;
}

int handle_greater_than() {
    if (c == '=') {
        sym.type = 24;
        strcpy(sym.content, ">=");
        c = getchar();
    }
    else {
        sym.type = 23;
        strcpy(sym.content, ">");
    }
    return 0;
}

int handle_equal() {
    if (c == '=') {
        sym.type = 25;
        strcpy(sym.content, "==");
        c = getchar();
    }
    else {
        sym.type = 27;
        strcpy(sym.content, "=");
    }
    return 0;
}

