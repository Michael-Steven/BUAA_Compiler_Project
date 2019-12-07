#ifndef GENERATE_MIPS_H
#define GENERATE_MIPS_H

#define REG_SUM 40 //without $k0 and $k1

typedef struct {
    char var[100];
    int reg;
    int type; // 0:int; 1:char;
    int stack_location;
} Store;

int is_space_s();

int is_space_t();

void flush_register(int location);

void add_var(char *var, char *value_reg, int type);

int find_var_location(char *var);

int find_const_value(char *con);

void mips_code_print(char *s);

void generate_mips();

void init_string();

void init_array(int location);

int generate_array(char *var, char *cal, char *write_reg, int mode);

void generate_read_statement(int location);

void generate_write_statement_1(int location);

void generate_write_statement_2(int location);

void generate_write_statement_3(int location);

void generate_calculate_statement(int location);

void generate_calculate_operand(int location, int num, char *cal, char *write_reg);

void generate_assignment_statement(int location);

void generate_condition_statement(int location);

void generate_jump_statement(int location);

void generate_function(int location);

void save_recursion_var(int prev_point, int now_point);

void reload_recursion_var();

void generate_return_statement(int location);

void generate_function_call(int location);

void generate_function_call_param(int location);

void generate_param(int location);

#endif //GENERATE_MIPS_H
