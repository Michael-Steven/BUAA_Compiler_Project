#ifndef GENERATE_MIDDLE_CODE_H
#define GENERATE_MIDDLE_CODE_H

typedef struct {
    int type;
    int c_len;
    int kind; // 0:int; 1:char;
    char code[6][100];
} Middle_code;

#define DECLARATION_HEADER          1  //函数声明头部
#define PARAMETER                   2  //函数参数
#define FUNCTION_CALL_PARAMETER     3  //函数调用参数
#define FUNCTION_CALL               4  //函数调用
#define FUNCTION_RETURN             5  //函数调用返回
#define RETURN_STATEMENT            6  //返回语句
#define VARIABLE_SPECIFICATION      7  //变量声明
#define CONSTANT_SPECIFICATION      8  //常量声明
#define ASSIGNMENT_STATEMENT        9  //赋值语句
#define CONDITION                   10 //条件判断
#define JUMP_STATEMENT              11 //跳转语句
#define READ_STATEMENT              12 //scanf
#define WRITE_STATEMENT_1           13 //printf(字符串， 表达式);
#define WRITE_STATEMENT_2           14 //printf(字符串);
#define WRITE_STATEMENT_3           15 //printf(表达式);
#define CALCULATE                   16 //计算语句
#define MAIN_FUNC                   17 //main函数
#define LABLE                       18 //标签

Middle_code middle_code_create(int type, int num, ...);

void middle_code_print();

#endif //GENERATE_MIDDLE_CODE_H
