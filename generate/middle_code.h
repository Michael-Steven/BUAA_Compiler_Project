#ifndef GENERATE_MIDDLE_CODE_H
#define GENERATE_MIDDLE_CODE_H

typedef struct middle_code {
    int type;
    char code[6][100];
} middle_code;

middle_code m_list[10000];
static int m_list_len = 0;

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
#define CALCULATE                   12 //计算语句

#endif //GENERATE_MIDDLE_CODE_H
