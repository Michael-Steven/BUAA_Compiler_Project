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
//static int var_stack_base = 0x7ff00000;
//static int var_stack_point = 0;
static int var_stack_point = 0x7ff00000;
Store store_list[10000];
static int store_len = 0;
Store roll_back_list[1000];
static int roll_back_len;
//static int stack_point = 0x7fffeffc;
static int lable_cnt = 0;
static int deep = 0;
static int dfs_deep = 0;
char cond_1[100], cond_op[100], cond_2[100]; //用于跳转语句的条件判断
char regs[35][10] = {"$0", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
                     "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
                     "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
                     "$t8", "$t9", "$gp", "$sp", "$fp", "$ra"};

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

void flush_register(int location) {
    int i, j, k, reg, flag;
    for (i = 0; i < store_len; i++) {
        reg = store_list[i].reg;
        if (reg >= 0) {
            flag = 0;
            for (j = location + 1; j < m_list_len; j++) {
                for (k = 0; k < m_list[j].c_len; k++) {
                    if (equal(m_list[j].code[k], store_list[i].var)) {
                        flag = 1;
                        break;
                    }
                }
                if (flag == 1) {
                    break;
                }
            }
            if (flag == 0) {
                store_list[i].reg = -2;
                usage[reg] = 0;
            }
        }
    }
}

void add_var(char *var, char *value_reg, int type) {
    int rs, rt, l;
    char out[1000];
    if ((l = find_var_location(var)) >= 0) {
        l = store_list[l].reg >= 0 ? store_list[l].reg : store_list[l].stack_location;
        if (l < REG_SUM) {
            sprintf(out, "move %s, %s", regs[l], value_reg);
            mips_code_print(out);
        }
        else {
            sprintf(out, "sw %s, 0x%x", value_reg, l);
            mips_code_print(out);
        }
    }
    else {
        rs = is_space_s();
        rt = is_space_t();
        if (rs >= 0) {
            strcpy(store_list[store_len].var, var);
            store_list[store_len].reg = rs;
            store_list[store_len].type = type;
            store_len++;
            usage[rs] = 1;
            sprintf(out, "move %s, %s", regs[rs], value_reg);
            mips_code_print(out);
        }
        else if (rt >= 0) {
            strcpy(store_list[store_len].var, var);
            store_list[store_len].reg = rt;
            store_list[store_len].type = type;
            store_len++;
            usage[rt] = 1;
            sprintf(out, "move %s, %s", regs[rt], value_reg);
            mips_code_print(out);
        }
        else {
            strcpy(store_list[store_len].var, var);
            store_list[store_len].reg = -1;
            store_list[store_len].type = type;
            var_stack_point -= 4;
            store_list[store_len].stack_location = var_stack_point;
            store_len++;
            sprintf(out, "sw %s, 0x%x", value_reg, var_stack_point);
            mips_code_print(out);
        }
    }
}

int find_var_location(char *var) {
    int i;
    for (i = 0; i < store_len; i++) {
        if (equal(store_list[i].var, var) && store_list[i].reg != -2) {
            return i;
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
    deep++;
    for (i = 0; m_list[i].type != FUNCTION_CALL; i++) {
        if (m_list[i].type == VARIABLE_SPECIFICATION && m_list[i].c_len == 4) {
            init_array(i);
        }
    }
    for (i = main_location + 2; i < m_list_len; i++) {
        if (m_list[i].type == VARIABLE_SPECIFICATION && m_list[i].c_len == 4) {
            init_array(i);
        }
        else if (m_list[i].type == READ_STATEMENT) {
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
        else if (m_list[i].type == CONDITION) {
            generate_condition_statement(i);
        }
        else if (m_list[i].type == JUMP_STATEMENT) {
            generate_jump_statement(i);
        }
        else if (m_list[i].type == LABLE) {
            mips_code_print(m_list[i].code[0]);
        }
        else if (m_list[i].type == FUNCTION_CALL_PARAMETER) {
            generate_function_call_param(i);
            while (m_list[i].type == FUNCTION_CALL_PARAMETER) i++;
            i--;
        }
        else if (m_list[i].type == FUNCTION_CALL) {
            generate_function_call(i);
        }
    }
    mips_code_print("li $v0, 10");
    mips_code_print("syscall");
    mips_code_print(" ");
    deep--;
    for (i = 0; i < main_location; i++) {
        if (m_list[i].type == DECLARATION_HEADER) {
            deep++;
            generate_function(i);
            deep--;
            i++;
            while (m_list[i].type != DECLARATION_HEADER && m_list[i].type != MAIN_FUNC) i++;
            i -= 2;
            mips_code_print(" ");
        }
        else if (m_list[i].type == LABLE) {
            mips_code_print(m_list[i].code[0]);
        }
    }
//    printf("end");
#endif
}

void init_string() {
    int i;
    char out[1000];
    for (i = 0; i < m_list_len; i++) {
        if (m_list[i].type == WRITE_STATEMENT_1 || m_list[i].type == WRITE_STATEMENT_2) {
            sprintf(out, "str_%d: .asciiz \"%s\"", lable_cnt, m_list[i].code[1]);
            sprintf(m_list[i].code[1], "str_%d", lable_cnt);
            lable_cnt++;
            mips_code_print(out);
        }
    }
    mips_code_print("enter: .asciiz \"\\n\"");
}

void init_array(int location) {
    strcpy(store_list[store_len].var, m_list[location].code[2]);
    store_list[store_len].reg = -1;
    store_list[store_len].type = m_list[location].kind;
    var_stack_point -= 4 * atoi(m_list[location].code[3]);
    store_list[store_len].stack_location = var_stack_point;
    store_len++;
}

int generate_array(char *var, char *cal, char *write_reg, int mode) { // 0:value; 1:address
    int i, j, l, ret = -1;
    char exp[100], out[100];
    for (i = 0; var[i] != '\0'; i++) {
        if (var[i] == '[') {
            ret = 0;
            strcpy(exp, &var[i + 1]);
            var[i] = '\0';
            for (j = strlen(exp) - 1; j >= 0; j--) {
                if (exp[j] == ']') {
                    exp[j] = '\0';
                    dfs_deep++;
                    generate_array(exp, cal, write_reg, mode);
                    dfs_deep--;
                    sprintf(out, "sll %s, %s, 2", write_reg, cal);
                    mips_code_print(out);
                    l = find_var_location(var);
                    l = store_list[l].reg >= 0 ? store_list[l].reg : store_list[l].stack_location;
                    sprintf(out, "addi %s, %s, 0x%x", write_reg, write_reg, l);
                    mips_code_print(out);
                    if (mode == 1) {
                        sprintf(out, "move $v1, %s", write_reg);
                        mips_code_print(out);
                    }
                    if (mode == 0 || dfs_deep != 0) {
                        sprintf(out, "lw %s, 0(%s)", write_reg, write_reg);
                        mips_code_print(out);
                        strcpy(cal, write_reg);
                    }
                    break;
                }
            }
            break;
        }
    }
    if (ret == -1 && dfs_deep != 0) {
        if (var[0] == '+' || var[0] == '-' || isdigit(var[0])) {
            sprintf(out, "li %s, 0x%x", write_reg, atoi(var));
            mips_code_print(out);
            strcpy(cal, write_reg);
        }
        else if ((l = find_const_value(var)) >= 0) {
            sprintf(out, "li %s, 0x%x", write_reg, atoi(m_list[l].code[4]));
            mips_code_print(out);
            strcpy(cal, write_reg);
        }
        else {
            l = find_var_location(var);
            l = store_list[l].reg >= 0 ? store_list[l].reg : store_list[l].stack_location;
            if (l < REG_SUM) {
                strcpy(cal, regs[l]);
            }
            else {
                sprintf(out, "lw %s, 0x%x", write_reg, l);
                mips_code_print(out);
                strcpy(cal, write_reg);
            }
        }
    }
    return ret;
}

void generate_read_statement(int location) {
    int l;
    char out[1000];
    if (m_list[location].kind == 0) {
        mips_code_print("li $v0, 5");
    }
    else {
        mips_code_print("li $v0, 12");
    }
    mips_code_print("syscall");
    if ((l = find_var_location(m_list[location].code[1])) >= 0) {
        l = store_list[l].reg >= 0 ? store_list[l].reg : store_list[l].stack_location;
    }
    if (l >= 0 && l < REG_SUM) {
        sprintf(out, "move %s, $v0", regs[l]);
        mips_code_print(out);
    }
    else if (l >= REG_SUM) {
        sprintf(out, "sw $v0, 0x%x", l);
        mips_code_print(out);
    }
    else {
        //flush_register(location);
        add_var(m_list[location].code[1], "$v0", m_list[location].kind);
    }
}

void generate_write_statement_1(int location) {
    int l;
    char out[1000], cal[100];
    generate_write_statement_2(location);
    if (m_list[location].code[2][0] == '\'') {
        mips_code_print("li $v0, 11");
        sprintf(out, "li $a0, %d", m_list[location].code[2][1]);
        mips_code_print(out);
        mips_code_print("syscall");
    }
    else {
        if (m_list[location].code[2][0] == '+' || m_list[location].code[2][0] == '-'
            || isdigit(m_list[location].code[2][0])) {
            mips_code_print("li $v0, 1");
            sprintf(out, "li $a0, 0x%x", atoi(m_list[location].code[2]));
        }
        else if (m_list[location].code[2][0] == '\'') {
            mips_code_print("li $v0, 11");
            sprintf(out, "li $a0, 0x%x", m_list[location].code[2][1]);
        }
        else if ((l = find_const_value(m_list[location].code[2])) >= 0) {
            if (equal(m_list[l].code[1], "int")) mips_code_print("li $v0, 1");
            else mips_code_print("li $v0, 11");
            sprintf(out, "li $a0, 0x%x", atoi(m_list[l].code[4])); //看看这样写有没有问题，字符型也用atoi
        }
        else {
            if (generate_array(m_list[location].code[2], cal, "$a0", 0) >= 0) {
                if (m_list[location].kind == 0) sprintf(out, "li $v0, 1");
                else
                    sprintf(out, "li $v0, 11");
            }
            else {
                l = find_var_location(m_list[location].code[2]);
                if (store_list[l].type == 0) mips_code_print("li $v0, 1");
                else mips_code_print("li $v0, 11");
                l = store_list[l].reg >= 0 ? store_list[l].reg : store_list[l].stack_location;
                if (l < 50) {
                    sprintf(out, "move $a0, %s", regs[l]);
                }
                else {
                    sprintf(out, "lw $a0, 0x%x", l);
                }
            }
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
    char out[1000], cal[100];
    int l;
    if (m_list[location].code[1][0] == '\'') {
        mips_code_print("li $v0, 11");
        sprintf(out, "li $a0, %d", m_list[location].code[1][1]);
        mips_code_print(out);
        mips_code_print("syscall");
    }
    else {
        if (m_list[location].code[1][0] == '+' || m_list[location].code[1][0] == '-'
            || isdigit(m_list[location].code[1][0])) {
            mips_code_print("li $v0, 1");
            sprintf(out, "li $a0, 0x%x", atoi(m_list[location].code[1]));
        }
        else if (m_list[location].code[1][0] == '\'') {
            mips_code_print("li $v0, 11");
            sprintf(out, "li $a0, 0x%x", m_list[location].code[1][1]);
        }
        else if ((l = find_const_value(m_list[location].code[1])) >= 0) {
            if (equal(m_list[l].code[1], "int")) mips_code_print("li $v0, 1");
            else mips_code_print("li $v0, 11");
            sprintf(out, "li $a0, 0x%x", atoi(m_list[l].code[4])); //看看这样写有没有问题，字符型也用atoi
        }
        else {
            if (generate_array(m_list[location].code[1], cal, "$a0", 0) >= 0) {
                if (m_list[location].kind == 0) sprintf(out, "li $v0, 1");
                else
                    sprintf(out, "li $v0, 11");
            }
            else {
                l = find_var_location(m_list[location].code[1]);
                if (store_list[l].type == 0) mips_code_print("li $v0, 1");
                else mips_code_print("li $v0, 11");
                l = store_list[l].reg >= 0 ? store_list[l].reg : store_list[l].stack_location;
                if (l < 50) {
                    sprintf(out, "move $a0, %s", regs[l]);
                }
                else {
                    sprintf(out, "lw $a0, 0x%x", l);
                }
            }
        }
        mips_code_print(out);
        mips_code_print("syscall");
    }
}

void generate_calculate_statement(int location) {
    char out[1000], cal_1[100], cal_2[100];
    int write_location = 0, flag = 0;
    if (generate_array(m_list[location].code[0], cal_1, "$t0", 1) >= 0) {
        flag = 1;
    }
    else if ((write_location = find_var_location(m_list[location].code[0])) >= 0) {
        write_location = store_list[write_location].reg >= 0 ? store_list[write_location].reg
                                                             : store_list[write_location].stack_location;
    }
    if (generate_array(m_list[location].code[2], cal_1, "$t0", 0) < 0)
        generate_calculate_operand(location, 2, cal_1, "$t0");
    if (generate_array(m_list[location].code[4], cal_2, "$t1", 0) < 0)
        generate_calculate_operand(location, 4, cal_2, "$t1");
    if (equal(m_list[location].code[3], "+")) {
        if (flag == 1) {
            sprintf(out, "add $t0, %s, %s", cal_1, cal_2);
            mips_code_print(out);
            mips_code_print("sw $t0, 0($v1)");
        }
        else if (write_location >= 0 && write_location < REG_SUM) {
            sprintf(out, "add %s, %s, %s", regs[write_location], cal_1, cal_2);
            mips_code_print(out);
        }
        else if (write_location >= REG_SUM) {
            sprintf(out, "add $t0, %s, %s", cal_1, cal_2);
            mips_code_print(out);
            sprintf(out, "sw $t0, 0x%x", write_location);
            mips_code_print(out);
        }
        else {
            sprintf(out, "add $t0, %s, %s", cal_1, cal_2);
            mips_code_print(out);
            //flush_register(location);
            add_var(m_list[location].code[0], "$t0", 0);
        }
    }
    else if (equal(m_list[location].code[3], "-")) {
        if (flag == 1) {
            sprintf(out, "sub $t0, %s, %s", cal_1, cal_2);
            mips_code_print(out);
            mips_code_print("sw $t0, 0($v1)");
        }
        else if (write_location >= 0 && write_location < REG_SUM) {
            sprintf(out, "sub %s, %s, %s", regs[write_location], cal_1, cal_2);
            mips_code_print(out);
        }
        else if (write_location >= REG_SUM) {
            sprintf(out, "sub $t0, %s, %s", cal_1, cal_2);
            mips_code_print(out);
            sprintf(out, "sw $t0, 0x%x", write_location);
            mips_code_print(out);
        }
        else {
            sprintf(out, "sub $t0, %s, %s", cal_1, cal_2);
            mips_code_print(out);
            //flush_register(location);
            add_var(m_list[location].code[0], "$t0", 0);
        }
    }
    else if (equal(m_list[location].code[3], "*")) {
        sprintf(out, "mult %s, %s", cal_1, cal_2);
        mips_code_print(out);
        if (flag == 1) {
            mips_code_print("mflo $t0");
            mips_code_print("sw $t0, 0($v1)");
        }
        else if (write_location >= 0 && write_location < REG_SUM) {
            sprintf(out, "mflo %s", regs[write_location]);
            mips_code_print(out);
        }
        else if (write_location >= REG_SUM) {
            mips_code_print("mflo $t0");
            sprintf(out, "sw $t0, 0x%x", write_location);
            mips_code_print(out);
        }
        else {
            mips_code_print("mflo $t0");
            //flush_register(location);
            add_var(m_list[location].code[0], "$t0", 0);
        }
    }
    else if (equal(m_list[location].code[3], "/")) {
        sprintf(out, "div %s, %s", cal_1, cal_2);
        mips_code_print(out);
        if (flag == 1) {
            mips_code_print("mflo $t0");
            mips_code_print("sw $t0, 0($v1)");
        }
        else if (write_location >= 0 && write_location < REG_SUM) {
            sprintf(out, "mflo %s", regs[write_location]);
            mips_code_print(out);
        }
        else if (write_location >= REG_SUM) {
            mips_code_print("mflo $t0");
            sprintf(out, "sw $t0, 0x%x", write_location);
            mips_code_print(out);
        }
        else {
            mips_code_print("mflo $t0");
            //flush_register(location);
            add_var(m_list[location].code[0], "$t0", 0);
        }
    }
}

void generate_calculate_operand(int location, int num, char *cal, char *write_reg) {
    char out[100];
    int l;
    if (m_list[location].code[num][0] == '+' || m_list[location].code[num][0] == '-'
        || isdigit(m_list[location].code[num][0])) {
        sprintf(out, "li %s, 0x%x", write_reg, atoi(m_list[location].code[num]));
        mips_code_print(out);
        strcpy(cal, write_reg);
    }
    else if (m_list[location].code[num][0] == '\'') {
        sprintf(out, "li %s, 0x%x", write_reg, m_list[location].code[num][1]);
        mips_code_print(out);
        strcpy(cal, write_reg);
    }
    else if ((l = find_const_value(m_list[location].code[num])) >= 0) {
        if (equal(m_list[l].code[1], "char")) {
            sprintf(out, "li %s, 0x%x", write_reg, m_list[l].code[4][0]);
        }
        else {
            sprintf(out, "li %s, 0x%x", write_reg, atoi(m_list[l].code[4]));
        }
        mips_code_print(out);
        strcpy(cal, write_reg);
    }
    else {
        l = find_var_location(m_list[location].code[num]);
        l = store_list[l].reg >= 0 ? store_list[l].reg : store_list[l].stack_location;
        if (l < REG_SUM) {
            strcpy(cal, regs[l]);
        }
        else {
            sprintf(out, "lw %s, 0x%x", write_reg, l);
            mips_code_print(out);
            strcpy(cal, write_reg);
        }
    }
}

void generate_assignment_statement(int location) {
    char out[1000], cal_1[1000];
    int write_location = 0, flag = 0;
//    write_location = find_var_location(m_list[location].code[0]);
    if (generate_array(m_list[location].code[0], cal_1, "$t0", 1) >= 0) {
        flag = 1;
    }
    else if ((write_location = find_var_location(m_list[location].code[0])) >= 0) {
        write_location = store_list[write_location].reg >= 0 ? store_list[write_location].reg
                                                             : store_list[write_location].stack_location;
    }
    if (equal(m_list[location].code[2], "RET")) {
        strcpy(cal_1, "$v0");
    }
    else {
        if (generate_array(m_list[location].code[2], cal_1, "$t0", 0) < 0)
            generate_calculate_operand(location, 2, cal_1, "$t0");
    }
    if (flag == 1) {
        sprintf(out, "sw %s, 0($v1)", cal_1);
        mips_code_print(out);
    }
    else if (write_location >= 0 && write_location < REG_SUM) {
//        store_list[write_location].type = m_list[location].kind;
        sprintf(out, "move %s, %s", regs[write_location], cal_1);
        mips_code_print(out);
    }
    else if (write_location >= REG_SUM) {
//        store_list[write_location].type = m_list[location].kind;
        sprintf(out, "sw %s, 0x%x", cal_1, write_location);
        mips_code_print(out);
    }
    else {
        //flush_register(location);
        add_var(m_list[location].code[0], cal_1, m_list[location].kind);
    }
}

void generate_condition_statement(int location) {
    if (generate_array(m_list[location].code[0], cond_1, "$t0", 0) < 0)
        generate_calculate_operand(location, 0, cond_1, "$t0");
    if (generate_array(m_list[location].code[2], cond_2, "$t1", 0) < 0)
        generate_calculate_operand(location, 2, cond_2, "$t1");
    strcpy(cond_op, m_list[location].code[1]);
}

void generate_jump_statement(int location) {
    char out[100];
    if (equal(m_list[location].code[0], "BNZ")) {
        if (equal(cond_op, "==")) {
            sprintf(out, "beq %s, %s, %s", cond_1, cond_2, m_list[location].code[1]);
        }
        else if (equal(cond_op, "!=")) {
            sprintf(out, "bne %s, %s, %s", cond_1, cond_2, m_list[location].code[1]);
        }
        else if (equal(cond_op, ">")) {
            sprintf(out, "bgt %s, %s, %s", cond_1, cond_2, m_list[location].code[1]);
        }
        else if (equal(cond_op, ">=")) {
            sprintf(out, "bge %s, %s, %s", cond_1, cond_2, m_list[location].code[1]);
        }
        else if (equal(cond_op, "<")) {
            sprintf(out, "blt %s, %s, %s", cond_1, cond_2, m_list[location].code[1]);
        }
        else if (equal(cond_op, "<=")) {
            sprintf(out, "ble %s, %s, %s", cond_1, cond_2, m_list[location].code[1]);
        }
    }
    else if (equal(m_list[location].code[0], "BZ")) {
        if (equal(cond_op, "==")) {
            sprintf(out, "bne %s, %s, %s", cond_1, cond_2, m_list[location].code[1]);
        }
        else if (equal(cond_op, "!=")) {
            sprintf(out, "beq %s, %s, %s", cond_1, cond_2, m_list[location].code[1]);
        }
        else if (equal(cond_op, ">")) {
            sprintf(out, "ble %s, %s, %s", cond_1, cond_2, m_list[location].code[1]);
        }
        else if (equal(cond_op, ">=")) {
            sprintf(out, "blt %s, %s, %s", cond_1, cond_2, m_list[location].code[1]);
        }
        else if (equal(cond_op, "<")) {
            sprintf(out, "bge %s, %s, %s", cond_1, cond_2, m_list[location].code[1]);
        }
        else if (equal(cond_op, "<=")) {
            sprintf(out, "bgt %s, %s, %s", cond_1, cond_2, m_list[location].code[1]);
        }
    }
    else if (equal(m_list[location].code[0], "GOTO")) {
        sprintf(out, "j %s", m_list[location].code[1]);
    }
    mips_code_print(out);
}

void generate_function(int location) {
    int i, prev_point = store_len;
    char func_name[100];
    strcpy(func_name, m_list[location].code[1]);
    for (i = location + 1; m_list[i + 1].type != DECLARATION_HEADER && m_list[i + 1].type != MAIN_FUNC; i++) {
        if (m_list[i].type == VARIABLE_SPECIFICATION && m_list[i].c_len == 4) {
            init_array(i);
        }
        else if (m_list[i].type == READ_STATEMENT) {
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
        else if (m_list[i].type == CONDITION) {
            generate_condition_statement(i);
        }
        else if (m_list[i].type == JUMP_STATEMENT) {
            generate_jump_statement(i);
        }
        else if (m_list[i].type == LABLE) {
            mips_code_print(m_list[i].code[0]);
        }
        else if (m_list[i].type == FUNCTION_CALL_PARAMETER) {
            generate_function_call_param(i);
            while (m_list[i].type == FUNCTION_CALL_PARAMETER) i++;
            i--;
        }
        else if (m_list[i].type == FUNCTION_CALL) {
            if (equal(func_name, m_list[i].code[1])) {
                save_recursion_var(prev_point, store_len);
                generate_function_call(i);
                reload_recursion_var();
                mips_code_print("lw $ra, 0($sp)");
                mips_code_print("addi $sp, $sp, 4");
            }
            else {
                generate_function_call(i);
                mips_code_print("lw $ra, 0($sp)");
                mips_code_print("addi $sp, $sp, 4");
            }
        }
        else if (m_list[i].type == RETURN_STATEMENT) {
            generate_return_statement(i);
        }
        else if (m_list[i].type == PARAMETER) {
            generate_param(i);
            while (m_list[i].type == PARAMETER) i++;
            i--;
        }
    }
}

void save_recursion_var(int prev_point, int now_point) {
    int i, var_reg;
    char out[100];
    for (i = now_point - 1; i >= prev_point; i--) {
        mips_code_print("addi $sp, $sp, -4");
//        stack_point -= 4;
        var_reg = store_list[i].reg;
        if (var_reg >= 0) {
            sprintf(out, "sw %s, 0($sp)", regs[var_reg]);
            mips_code_print(out);
        }
        else {
            sprintf(out, "lw $t0, 0x%x", store_list[i].stack_location);
            mips_code_print(out);
            mips_code_print("sw $t0, 0($sp)");
        }
    }
    roll_back_len = 0;
    for (i = prev_point; i < now_point; i++) {
        roll_back_list[roll_back_len++] = store_list[i];
    }
}

void reload_recursion_var() {
    int i;
    for (i = 0; i < roll_back_len; i++) {
        mips_code_print("lw $t0, 0($sp)");
        mips_code_print("addi $sp, $sp, 4");
        add_var(roll_back_list[i].var, "$t0", roll_back_list[i].type);
    }
}

void generate_return_statement(int location) {
    char ret[100], out[100];
    if (m_list[location].c_len == 1) {
        mips_code_print("jr $ra");
    }
    else if (m_list[location].c_len == 2) {
        generate_calculate_operand(location, 1, ret, "$v0");\
        if (!equal(ret, "$v0")) {
            sprintf(out, "move $v0, %s", ret);
            mips_code_print(out);
        }
        mips_code_print("jr $ra");
    }
}

void generate_function_call(int location) {
    char out[100];
    sprintf(out, "jal %s", m_list[location].code[1]);
    mips_code_print(out);
}

void generate_function_call_param(int location) {
    char out[100], cal[100], write_reg[100];
    int i, cnt = 0;
    mips_code_print("addi $sp, $sp, -4");
    mips_code_print("sw $ra, 0($sp)");
    for (i = location; m_list[i].type == FUNCTION_CALL_PARAMETER; i++) {
        if (cnt < 4) {
            sprintf(write_reg, "$a%d", cnt);
            generate_calculate_operand(i, 1, cal, write_reg);
            if (!equal(cal, write_reg)) {
                sprintf(out, "move %s, %s", write_reg, cal);
                mips_code_print(out);
            }
        }
        cnt++;
    }
    for (i = cnt; i > 4; i--) {
        generate_calculate_operand(location + i - 1, 1, cal, "$t0");
        if (!equal(cal, write_reg)) {
            sprintf(out, "move $t0, %s", cal);
            mips_code_print(out);
        }
        mips_code_print("addi $sp, $sp, -4");
        mips_code_print("sw $t0, 0($sp)");
        mips_code_print(out);
    }
}

void generate_param(int location) {
    char out[100], read_reg[100];
    int i, cnt = 0;
    for (i = location; m_list[i].type == PARAMETER; i++) {
        if (cnt < 4) {
            sprintf(read_reg, "$a%d", cnt);
            //flush_register(i);
            add_var(m_list[i].code[2], read_reg, m_list[i].kind);
        }
        else {
            mips_code_print("lw $t0, 0($sp)");
            mips_code_print("addi $sp, $sp, 4");
            mips_code_print(out);
            //flush_register(i);
            add_var(m_list[i].code[2], "$t0", m_list[i].kind);
        }
        cnt++;
    }
}