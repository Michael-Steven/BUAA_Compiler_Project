#ifndef GENERATE_ERROR_H
#define GENERATE_ERROR_H

#define check(A)         { r = (A); if (r.ret < 0) { return r;}}
#define equal(A, B)      (strcmp(A, B) == 0)

void handle_error(char err[]);

#endif //GENERATE_ERROR_H
