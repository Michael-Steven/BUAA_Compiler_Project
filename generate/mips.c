#include "file.h"
#include "mips.h"
#include "middle_code.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

extern Middle_code m_list[10000];
extern int m_list_len;
extern int main_location;
int usage[35]; //寄存器是否被使用 0: 否; 1: 是
static int stack_point = 0x7fffeffc;
Store store_list[10000];
static int store_len = 0;
static int lable_cnt = 0;
static int deep = 0;
char regs[35][10] = {"$0", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
                     "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
                     "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
                     "$t8", "$t9", "$gp", "$sp", "$fp", "$ra"};

int is_space_a() {
    int i;
    for (i = 4; i <= 7; i++) {
        if (usage[i] == 0) {
            return i;
        }
    }
    return -1;
}

int is_space_s() {
    int i;
    for (i = 16; i <= 23; i++) {
        if (usage[i] == 0) {
            return i;
        }
    }
    return -1;
}

int is_space_t() {
    int i;
    for (i = 10; i <= 25; i++) {
        if (usage[i] == 0) {
            return i;
        }
        while (i >= 15 && i <= 22) {
            i++;
        }
    }
    return -1;
}

void add_var(char *var, char *value_reg) {
    int rs, rt;
    char out[1000];
    rs = is_space_s();
    rt = is_space_t();
    if (rs >= 0) {
        strcpy(store_list[store_len].var, var);
        store_list[store_len].reg = rs;
        store_len++;
        usage[rs] = 1;
        sprintf(out, "move %s, %s", regs[rs], value_reg);
        mips_code_print(out);
    }
    else if (rt >= 0) {
        strcpy(store_list[store_len].var, var);
        store_list[store_len].reg = rt;
        store_len++;
        usage[rt] = 1;
        sprintf(out, "move %s, %s", regs[rt], value_reg);
        mips_code_print(out);
    }
    else {
        strcpy(store_list[store_len].var, var);
        store_list[store_len].reg = -1;
        stack_point -= 4;
        store_list[store_len].stack_location = stack_point;
        store_len++;
        sprintf(out, "sw %s, 0x%x", value_reg, stack_point);
        mips_code_print(out);
    }
}

int find_var_location(char *var) {
    int i;
    for (i = 0; i < store_len; i++) {
        if (equal(store_list[i].var, var)) {
            return store_list[i].reg >= 0 ? store_list[i].reg : store_list[i].stack_location;
        }
    }
    return -1;
}

int find_const_value(char *con) {
    int i;
    for (i = 0; i < m_list_len; i++) {
        if (m_list[i].type == CONSTANT_SPECIFICATION && equal(m_list[i].code[2], con)) {
            return i;
        }
    }
    return -1;
}

void mips_code_print(char *s) {
    int i;
    for (i = 0; i < deep; i++) {
        fprintf(mips_code_out, "    ");
    }
    fprintf(mips_code_out, "%s\n", s);
}

void generate_mips() {
#ifdef MIPS_CODE_OUT
    int i;
    mips_code_print(".data");
    deep++;
    init_string();
    deep--;
    mips_code_print("\n.text");
    deep++;
    mips_code_print("main:");
    for (i = main_location + 1; i < m_list_len; i++) {
        if (m_list[i].type == READ_STATEMENT) {
            generate_read_statement(i);
        }
        else if (m_list[i].type == WRITE_STATEMENT_1) {
            generate_write_statement_1(i);
            mips_code_print("li $v0, 4");
            mips_code_print("la $a0, enter");
            mips_code_print("syscall");
        }
        else if (m_list[i].type == WRITE_STATEMENT_2) {
            generate_write_statement_2(i);
            mips_code_print("li $v0, 4");
            mips_code_print("la $a0, enter");
            mips_code_print("syscall");
        }
        else if (m_list[i].type == WRITE_STATEMENT_3) {
            generate_write_statement_3(i);
            mips_code_print("li $v0, 4");
            mips_code_print("la $a0, enter");
            mips_code_print("syscall");
        }
        else if (m_list[i].type == CALCULATE) {
            generate_calculate_statement(i);
        }
        else if (m_list[i].type == ASSIGNMENT_STATEMENT) {
            generate_assignment_statement(i);
        }
    }
    mips_code_print("li $v0, 10");
    mips_code_print("syscall");
    for (i = 0; i < m_list_len; i++) {
        if (m_list[i].type == MAIN_FUNC) {
            break;
        }
    }
#endif
}

void init_string() {
    int i;
    char out[1000];
    for (i = 0; i < m_list_len; i++) {
        if (m_list[i].type == WRITE_STATEMENT_1 || m_list[i].type == WRITE_STATEMENT_2) {
            sprintf(out, "lable_%d: .asciiz \"%s\"", lable_cnt, m_list[i].code[1]);
            sprintf(m_list[i].code[1], "lable_%d", lable_cnt);
            lable_cnt++;
            mips_code_print(out);
        }
    }
    mips_code_print("enter: .asciiz \"\\n\"");
}

void generate_read_statement(int location) {
    int l;
    char out[1000];
    mips_code_print("li $v0, 5");
    mips_code_print("syscall");
    l = find_var_location(m_list[location].code[1]);
    if (l >= 0 && l < 50) {
        sprintf(out, "move %s, $v0", regs[l]);
        mips_code_print(out);
    }
    else if (l >= 50) {
        sprintf(out, "sw $v0, 0x%x", l);
        mips_code_print(out);
    }
    else {
        add_var(m_list[location].code[1], "$v0");
    }
}

void generate_write_statement_1(int location) {
    int l;
    char out[1000];
    generate_write_statement_2(location);
    if (m_list[location].code[2][0] == '\'') {
        mips_code_print("li $v0, 11");
        sprintf(out, "li $a0, %d", m_list[location].code[2][1]);
        mips_code_print(out);
        mips_code_print("syscall");
    }
    else {
        mips_code_print("li $v0, 1");
        if ((l = find_const_value(m_list[location].code[2])) >= 0) {
            sprintf(out, "li $a0, 0x%x", atoi(m_list[l].code[4]));
        }
        else if ((l = find_var_location(m_list[location].code[2])) < 50) {
            sprintf(out, "move $a0, %s", regs[l]);
        }
        else {
            sprintf(out, "lw $a0, 0x%x", l);
        }
        mips_code_print(out);
        mips_code_print("syscall");
    }
}

void generate_write_statement_2(int location) {
    char out[1000];
    mips_code_print("li $v0, 4");
    sprintf(out, "la $a0, %s", m_list[location].code[1]);
    mips_code_print(out);
    mips_code_print("syscall");
}

void generate_write_statement_3(int location) {
    char out[1000];
    int l;
    if (m_list[location].code[1][0] == '\'') {
        mips_code_print("li $v0, 11");
        sprintf(out, "li $a0, %d", m_list[location].code[1][1]);
        mips_code_print(out);
        mips_code_print("syscall");
    }
    else {
        mips_code_print("li $v0, 1");
        if ((l = find_const_value(m_list[location].code[1])) >= 0) {
            sprintf(out, "li $a0, 0x%x", atoi(m_list[l].code[4]));
        }
        else if ((l = find_var_location(m_list[location].code[1])) < 50) {
            sprintf(out, "move $a0, %s", regs[l]);
        }
        else {
            sprintf(out, "lw $a0, 0x%x", l);
        }
        mips_code_print(out);
        mips_code_print("syscall");
    }
}

void generate_calculate_statement(int location) {
    char out[1000], cal_1[100], cal_2[100], r[100];
    int l, write_location = 0;
    write_location = find_var_location(m_list[location].code[0]);
    if (m_list[location].code[2][0] == '+' || m_list[location].code[2][0] == '-'
        || isdigit(m_list[location].code[2][0])) {
        sprintf(out, "li $t0, 0x%x", atoi(m_list[location].code[2]));
        mips_code_print(out);
        strcpy(cal_1, "$t0");
    }
    else if (m_list[location].code[2][0] == '\'') {
        sprintf(out, "li $t0, 0x%x", m_list[location].code[2][1]);
        mips_code_print(out);
        strcpy(cal_1, "$t0");
    }
    else if ((l = find_const_value(m_list[location].code[2])) >= 0) {
        if (equal(m_list[l].code[1], "char")) {
            sprintf(out, "li $t0, 0x%x", m_list[l].code[4][0]);
        }
        else {
            sprintf(out, "li $t0, 0x%x", atoi(m_list[l].code[4]));
        }
        mips_code_print(out);
        strcpy(cal_1, "$t0");
    }
    else if ((l = find_var_location(m_list[location].code[2])) < 50) {
        strcpy(cal_1, regs[l]);
    }
    else {
        sprintf(out, "lw $t0, 0x%x", l);
        mips_code_print(out);
        strcpy(cal_1, "$t0");
    }
    if (m_list[location].code[4][0] == '+' || m_list[location].code[4][0] == '-'
        || isdigit(m_list[location].code[4][0])) {
        sprintf(out, "li $t1, 0x%x", atoi(m_list[location].code[4]));
        mips_code_print(out);
        strcpy(cal_2, "$t1");
    }
    else if (m_list[location].code[4][0] == '\'') {
        sprintf(out, "li $t1, 0x%x", m_list[location].code[4][1]);
        mips_code_print(out);
        strcpy(cal_2, "$t1");
    }
    else if ((l = find_const_value(m_list[location].code[4])) >= 0) {
        if (equal(m_list[l].code[1], "char")) {
            sprintf(out, "li $t1, 0x%x", m_list[l].code[4][0]);
        }
        else {
            sprintf(out, "li $t1, 0x%x", atoi(m_list[l].code[4]));
        }
        mips_code_print(out);
        strcpy(cal_2, "$t1");
    }
    else if ((l = find_var_location(m_list[location].code[4])) < 50) {
        strcpy(cal_2, regs[l]);
    }
    else {
        sprintf(out, "lw $t1, 0x%x", l);
        mips_code_print(out);
        strcpy(cal_2, "$t1");
    }
    if (equal(m_list[location].code[3], "+")) {
        if (write_location >= 0 && write_location < 50) {
            sprintf(out, "add %s %s %s", regs[write_location], cal_1, cal_2);
            mips_code_print(out);
        }
        else if (write_location >= 50) {
            sprintf(out, "add $t0 %s %s", cal_1, cal_2);
            mips_code_print(out);
            sprintf(out, "sw $t0, 0x%x", write_location);
            mips_code_print(out);
        }
        else {
            sprintf(out, "add $t0 %s %s", cal_1, cal_2);
            mips_code_print(out);
            add_var(m_list[location].code[0], "$t0");
        }
    }
    else if (equal(m_list[location].code[3], "-")) {
        if (write_location >= 0 && write_location < 50) {
            sprintf(out, "sub %s %s %s", regs[write_location], cal_1, cal_2);
            mips_code_print(out);
        }
        else if (write_location >= 50) {
            sprintf(out, "sub $t0 %s %s", cal_1, cal_2);
            mips_code_print(out);
            sprintf(out, "sw $t0, 0x%x", write_location);
            mips_code_print(out);
        }
        else {
            sprintf(out, "sub $t0 %s %s", cal_1, cal_2);
            mips_code_print(out);
            add_var(m_list[location].code[0], "$t0");
        }
    }
    else if (equal(m_list[location].code[3], "*")) {
        sprintf(out, "mult %s %s", cal_1, cal_2);
        mips_code_print(out);
        if (write_location >= 0 && write_location < 50) {
            sprintf(out, "mflo %s", regs[write_location]);
            mips_code_print(out);
        }
        else if (write_location >= 50) {
            mips_code_print("mflo $t0");
            sprintf(out, "sw $t0, 0x%x", write_location);
            mips_code_print(out);
        }
        else {
            mips_code_print("mflo $t0");
            add_var(m_list[location].code[0], "$t0");
        }
    }
    else if (equal(m_list[location].code[3], "/")) {
        sprintf(out, "div %s %s", cal_1, cal_2);
        mips_code_print(out);
        if (write_location >= 0 && write_location < 50) {
            sprintf(out, "mflo %s", regs[write_location]);
            mips_code_print(out);
        }
        else if (write_location >= 50) {
            mips_code_print("mflo $t0");
            sprintf(out, "sw $t0, 0x%x", write_location);
            mips_code_print(out);
        }
        else {
            mips_code_print("mflo $t0");
            add_var(m_list[location].code[0], "$t0");
        }
    }
}

void generate_assignment_statement(int location) {
    char out[1000], cal_1[1000];
    int l, write_location;
    write_location = find_var_location(m_list[location].code[0]);
    if (m_list[location].code[2][0] == '+' || m_list[location].code[2][0] == '-'
        || isdigit(m_list[location].code[2][0])) {
        sprintf(out, "li $t0, 0x%x", atoi(m_list[location].code[2]));
        mips_code_print(out);
        strcpy(cal_1, "$t0");
    }
    else if ((l = find_const_value(m_list[location].code[2])) >= 0) {
        sprintf(out, "li $t0, 0x%x", atoi(m_list[l].code[4]));
        mips_code_print(out);
        strcpy(cal_1, "$t0");
    }
    else if ((l = find_var_location(m_list[location].code[2])) < 50) {
        strcpy(cal_1, regs[l]);
    }
    else {
        sprintf(out, "lw $t0, 0x%x", l);
        mips_code_print(out);
        strcpy(cal_1, "$t0");
    }

    if (write_location >= 0 && write_location < 50) {
        sprintf(out, "move %s %s", regs[write_location], cal_1);
        mips_code_print(out);
    }
    else if (write_location >= 50) {
        sprintf(out, "sw %s, 0x%x", cal_1, write_location);
        mips_code_print(out);
    }
    else {
        add_var(m_list[location].code[0], cal_1);
    }
}