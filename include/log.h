#pragma once

#include <stdbool.h>
#include <stdio.h>

extern bool log_enabled;

#define LOG_BASE(print_func, output, ...)                                   \
if (log_enabled) {                                                          \
    print_func((output), "[LOG] ");                                         \
    print_func((output), "At " __FILE__ ":%d (%s)\n", __LINE__, __func__);  \
    print_func((output), "[LOG] ");                                         \
    print_func((output), __VA_ARGS__);                                      \
    print_func((output), "\n");                                             \
}

#define LOG(...) LOG_BASE(fprintf, stdout, __VA_ARGS__)
#define ERR(...) LOG_BASE(fprintf, stderr, __VA_ARGS__)
#define LOGFILE(file, ...) LOG_BASE(fprintf, file, __VA_ARGS__)
