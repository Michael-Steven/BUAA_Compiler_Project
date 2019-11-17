#include <stdio.h>
#include "file.h"

void handle_error(char err[]) {
    fprintf(error, "%s\n", err);
}
