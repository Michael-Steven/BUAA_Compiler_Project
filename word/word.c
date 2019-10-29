#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "word.h"

int main() {
    freopen("testfile.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    c = getchar();
    while (1) {
        while (isspace(c)) {
            c = getchar();
        }
        if (c == EOF) break;
        gettoken();
        printf("%s %s\n", mark[word.type], word.content);
    }
    return 0;
}

void gettoken() {
    if (isalpha(c) || c == '_') {
        handle_IDENFR();
    }
    else if (isdigit(c)) {
        handle_INTCON();
    }
    else if (c == '\'') {
        handle_CHARCON();
    }
    else if (c == '\"') {
        handle_STRCON();
    }
    else if (c == '+') {
        word.type = 17;
        strcpy(word.content, "+");
        c = getchar();
    }
    else if (c == '-') {
        word.type = 18;
        strcpy(word.content, "-");
        c = getchar();
    }
    else if (c == '*') {
        word.type = 19;
        strcpy(word.content, "*");
        c = getchar();
    }
    else if (c == '/') {
        word.type = 20;
        strcpy(word.content, "/");
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
            word.type = 26;
            strcpy(word.content, "!=");
            c = getchar();
        }
        else {
            handle_error("!= error");
        }
    }
    else if (c == ';') {
        word.type = 28;
        strcpy(word.content, ";");
        c = getchar();
    }
    else if (c == ',') {
        word.type = 29;
        strcpy(word.content, ",");
        c = getchar();
    }
    else if (c == '(') {
        word.type = 30;
        strcpy(word.content, "(");
        c = getchar();
    }
    else if (c == ')') {
        word.type = 31;
        strcpy(word.content, ")");
        c = getchar();
    }
    else if (c == '[') {
        word.type = 32;
        strcpy(word.content, "[");
        c = getchar();
    }
    else if (c == ']') {
        word.type = 33;
        strcpy(word.content, "]");
        c = getchar();
    }
    else if (c == '{') {
        word.type = 34;
        strcpy(word.content, "{");
        c = getchar();
    }
    else if (c == '}') {
        word.type = 35;
        strcpy(word.content, "}");
        c = getchar();
    }
}

void handle_error(char err[]) {
    printf("%s\n", err);
    exit(0);
}

void handle_IDENFR() {
    int i = 0;
    s[i++] = c;
    while ((c = getchar())) {
        if (!isalnum(c) && c != '_') {
            break;
        }
        s[i++] = c;
    }
    s[i] = '\0';
    if (strcmp(s, "const") == 0) {
        word.type = 4;
        strcpy(word.content, "const");
    }
    else if (strcmp(s, "int") == 0) {
        word.type = 5;
        strcpy(word.content, "int");
    }
    else if (strcmp(s, "char") == 0) {
        word.type = 6;
        strcpy(word.content, "char");
    }
    else if (strcmp(s, "void") == 0) {
        word.type = 7;
        strcpy(word.content, "void");
    }
    else if (strcmp(s, "main") == 0) {
        word.type = 8;
        strcpy(word.content, "main");
    }
    else if (strcmp(s, "if") == 0) {
        word.type = 9;
        strcpy(word.content, "if");
    }
    else if (strcmp(s, "else") == 0) {
        word.type = 10;
        strcpy(word.content, "else");
    }
    else if (strcmp(s, "do") == 0) {
        word.type = 11;
        strcpy(word.content, "do");
    }
    else if (strcmp(s, "while") == 0) {
        word.type = 12;
        strcpy(word.content, "while");
    }
    else if (strcmp(s, "for") == 0) {
        word.type = 13;
        strcpy(word.content, "for");
    }
    else if (strcmp(s, "scanf") == 0) {
        word.type = 14;
        strcpy(word.content, "scanf");
    }
    else if (strcmp(s, "printf") == 0) {
        word.type = 15;
        strcpy(word.content, "printf");
    }
    else if (strcmp(s, "return") == 0) {
        word.type = 16;
        strcpy(word.content, "return");
    }
    else {
        word.type = 0;
        strcpy(word.content, s);
    }
}

void handle_INTCON() {
    int i = 0;
    s[i++] = c;
    while ((c = getchar())) {
        if (!isdigit(c)) {
            break;
        }
        s[i++] = c;
    }
    s[i] = '\0';
    word.type = 1;
    strcpy(word.content, s);
}

void handle_CHARCON() {
    int i = 0;
    c = getchar();
    s[i++] = c;
    s[i] = '\0';
    word.type = 2;
    strcpy(word.content, s);
    c = getchar();
    if (c != '\'') {
        handle_error("handle_CHARCON error");
    }
    c = getchar();
}

void handle_STRCON() {
    int i = 0;
    while ((c = getchar())) {
        if (c == '\"') {
            break;
        }
        s[i++] = c;
    }
    s[i] = '\0';
    word.type = 3;
    strcpy(word.content, s);
    if (c != '\"') {
        handle_error("handle_STRCON error");
    }
    c = getchar();
}

void handle_less_than() {
    if (c == '=') {
        word.type = 22;
        strcpy(word.content, "<=");
        c = getchar();
    }
    else {
        word.type = 21;
        strcpy(word.content, "<");
    }
}

void handle_greater_than() {
    if (c == '=') {
        word.type = 24;
        strcpy(word.content, ">=");
        c = getchar();
    }
    else {
        word.type = 23;
        strcpy(word.content, ">");
    }
}

void handle_equal() {
    if (c == '=') {
        word.type = 25;
        strcpy(word.content, "==");
        c = getchar();
    }
    else {
        word.type = 27;
        strcpy(word.content, "=");
    }
}

