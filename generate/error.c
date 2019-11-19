#include <stdio.h>
#include "file.h"

void handle_error(char err[]) {
#ifdef ERROR_OUT
    fprintf(error_out, "%s\n", err);
#endif
}
