#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "../include/error.h"

int errprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int n = vfprintf(stderr, fmt, args);
    if (errno != 0) {
        n += fprintf(stderr, ": %s (errno %i)\n", strerror(errno), errno);
    }
    va_end(args);
    return n;
}
