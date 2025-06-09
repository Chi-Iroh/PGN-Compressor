#pragma once

#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>

#include "../include/log.h"

#define SOURCE_LOCATION_STACK_MAX 32

typedef uint32_t line_number; // C99 §6.10.4 (Line control) says it's undefined behavior to have more than 2147483647 lines, it fits in an int32_t, but uint32_t won't heart
#define PRINTF_LINE_NUMBER_FLAG "%" PRId32

struct _source_location {
    const char* file;
    const char* func;
    line_number line;
};

struct source_location {
    const char* file;
    const char* func;
    line_number line;
    size_t strsize; // Size of only this location, ignoring its possible callers
    struct _source_location call_stack[SOURCE_LOCATION_STACK_MAX];
    size_t call_stack_size;
    size_t callers_str_cumulative_size; // Cumulative size of all callers when converted to string
};

// loc_here(NULL) if no caller
struct source_location loc_here(const struct source_location* caller, const char* file, const char* func, line_number line);

#define LOC_HERE_FROM(caller) loc_here((caller), __FILE__, __func__, __LINE__)
#define LOC_HERE loc_here(NULL, __FILE__, __func__, __LINE__)

const char* loc_str(struct source_location loc);
void loc_exit(void);

#define LOG_BASE_FROM(loc, print_func, output, ...)             \
if (log_enabled) {                                              \
/* loc_str inserts [LOG] + \n on each line of the call stack */ \
    print_func((output), "%s", loc_str((loc)));                 \
    print_func((output), "[LOG] ");                             \
    print_func((output), __VA_ARGS__);                          \
    print_func((output), "\n");                                 \
}

#define LOG_FROM(loc, ...) LOG_BASE_FROM(loc, fprintf, stdout, __VA_ARGS__)
#define ERR_FROM(loc, ...) LOG_BASE_FROM(loc, fprintf, stderr, __VA_ARGS__)
#define LOGFILE_FROM(loc, file, ...) LOG_BASE_FROM(loc, fprintf, file, __VA_ARGS__)
