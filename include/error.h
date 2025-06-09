#pragma once
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// perror but with a printf-like formatting string instead of a fixed string
int errprintf(const char* fmt, ...) __attribute__((format (printf, 1, 2)));

#define ASSERT_PRINTF_BASE(condition, fail_statement, /* fmt, */ ...) {                          \
    if (!(condition)) {                                                     \
        errprintf("Assertion failed at %s:%u :\n\t", __FILE__, __LINE__);   \
        errprintf(__VA_ARGS__);                                             \
        fputs("\n", stderr);                                                \
        fail_statement;                                                     \
    }                                                                       \
}


#define ASSERT_PRINTF_EXIT_PROGRAM(condition, /* fmt, */ ...) ASSERT_PRINTF_BASE(condition, _Exit(EXIT_FAILURE), __VA_ARGS__)
#define ASSERT_PRINTF_EXIT_FAILURE(condition, /* fmt, */ ...) ASSERT_PRINTF_BASE(condition, return EXIT_FAILURE, __VA_ARGS__)
#define ASSERT_PRINTF_NULL(condition, /* fmt, */ ...) ASSERT_PRINTF_BASE(condition, return NULL, __VA_ARGS__)
#define ASSERT_PRINTF(condition, /* fmt, */ ...) ASSERT_PRINTF_BASE(condition, return false, __VA_ARGS__)
#define ASSERT_PRINTF_RETURN(condition, /* fmt, */ ...) ASSERT_PRINTF_BASE(condition, return, __VA_ARGS__)
#define FAIL(/* fmt, */ ...) ASSERT_PRINTF_EXIT_PROGRAM(false, __VA_ARGS__)
