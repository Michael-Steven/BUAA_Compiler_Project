#ifndef GENERATE_MIPS_H
#define GENERATE_MIPS_H

#define REG_SUM 30 //without $k0 and $k1

typedef struct {
    char var[100];
    int reg;
    int stack_location;
} Store;

int is_space_a();

int is_space_s();

int is_space_t();

void add_var(char *var, char *value_reg);

int find_var_location(char *var);

int find_const_value(char *con);

void mips_code_print(char *s);

void generate_mips();

void init_string();

void generate_read_statement(int location);

void generate_write_statement_1(int location);

void generate_write_statement_2(int location);

void generate_write_statement_3(int location);

void generate_calculate_statement(int location);

void generate_assignment_statement(int location);

#endif //GENERATE_MIPS_H
