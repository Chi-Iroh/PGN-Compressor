#pragma once

#include <stdbool.h>
#include <stdio.h>

extern bool log_enabled;

#define LOG_BASE(show_location, print_func, output, ...)                        \
if (log_enabled) {                                                              \
    if ((show_location)) {                                                      \
        print_func((output), "[LOG] ");                                         \
        print_func((output), "At " __FILE__ ":%d (%s)\n", __LINE__, __func__);  \
    }                                                                           \
    print_func((output), "[LOG] ");                                             \
    print_func((output), __VA_ARGS__);                                          \
    print_func((output), "\n");                                                 \
}

#define LOG_BASE_WITH_LOCATION(print_func, output, ...) LOG_BASE(true, print_func, output, __VA_ARGS__)
#define LOG_BASE_NO_LOCATION(print_func, output, ...) LOG_BASE(false, print_func, output, __VA_ARGS__)

#define LOG(...) LOG_BASE_WITH_LOCATION(fprintf, stdout, __VA_ARGS__)
#define ERR(...) LOG_BASE_WITH_LOCATION(fprintf, stderr, __VA_ARGS__)
#define LOGFILE(file, ...) LOG_BASE_WITH_LOCATION(fprintf, file, __VA_ARGS__)
#define LOGFILE_NO_LOCATION(file, ...) LOG_BASE_NO_LOCATION(fprintf, file, __VA_ARGS__)
