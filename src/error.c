#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "../include/error.h"

int errprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const int n = vfprintf(stderr, fmt, args);
    return n + fprintf(stderr, ": %s\n", strerror(errno));
}
