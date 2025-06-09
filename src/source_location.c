#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../include/error.h"
#include "../include/log.h"
#include "../include/source_location.h"

static unsigned char count_digits(line_number n) {
    unsigned char count = 0;

    if (n < 0) {
        count++;
        n = -n;
    }
    do {
        count++;
        n /= 10;
    } while (n != 0);
    return count;
}

// includes [LOG], \n & \0
static size_t _compute_loc_size(const struct _source_location* loc, bool is_caller) {
    ASSERT_PRINTF_EXIT_PROGRAM(loc != NULL, "NULL source location !");

    return
        6 +                         // "[LOG] "
        (is_caller ? 5 : 3) +       // "From " (if caller), or "At " if not
        strlen(loc->file) +         // file
        1 +                         // :
        count_digits(loc->line) +   // line
        2 +                         // " ("
        strlen(loc->func) +         // func
        1 +                         // ")"
        1 +                         // "\n"
        1;                          // \0
}

// includes \0
static inline size_t compute_loc_size(const struct source_location* loc, bool is_caller) {
    return _compute_loc_size((const struct _source_location*)loc, is_caller);
}

// *str is filled with \0
// new_size must be large enough to hold a trailing \0
void alloc_or_realloc(char** str, size_t old_size, size_t new_size) {
    ASSERT_PRINTF_EXIT_PROGRAM(str != NULL, "NULL str pointer !");

    if (*str == NULL) {
        *str = calloc(new_size, sizeof(char));
        if (*str == NULL) {
            ERR("Cannot allocate %zu byte%s !", new_size, (new_size > 1) ? "s" : "");
            return;
        }
    } else if (old_size < new_size) {
        char* const newstr = realloc(*str, new_size * sizeof(char));
        if (newstr == NULL) {
            ERR("Cannot reallocate %zu byte%s !", new_size, (new_size > 1) ? "s" : "");
            free(*str);
            *str = NULL;
            return;
        }
        *str = newstr;
        memset(*str, 0, new_size * sizeof(char)); // filling with \0
    }
}

static char* _loc_str_ptr = NULL;

const char* _loc_str(struct _source_location loc, bool is_caller) {
    static char* str = NULL;
    static size_t size = 0;
    const size_t locsize = _compute_loc_size(&loc, is_caller);

    alloc_or_realloc(&str, size, locsize);
    sprintf(str, "[LOG] %s %s:" PRINTF_LINE_NUMBER_FLAG " (%s)\n", (is_caller) ? "From" : "At", loc.file, loc.line, loc.func);
    size = locsize;
    return str;
}

static char* loc_str_ptr = NULL;

const char* loc_str(struct source_location loc) {
    static char* str = NULL;
    size_t size = 0;

    const size_t locsize = loc.callers_str_cumulative_size + loc.strsize;

    alloc_or_realloc(&str, size, locsize);
    for (unsigned i = 0; i < loc.call_stack_size; i++) {
        const char* caller_str = _loc_str(*(const struct _source_location*)&loc.call_stack[i], true);
        strcat(str, caller_str);
    }
    const char* const s = _loc_str(*(const struct _source_location*)&loc, false);
    strcat(str, s);
    return str;
}

struct source_location loc_here(const struct source_location* caller, const char* file, const char* func, line_number line) {
    struct source_location loc = {
        .file = file,
        .func = func,
        .line = line,
        .strsize = 0,
        .call_stack = { 0 },
        .call_stack_size = 0,
        .callers_str_cumulative_size = 0
    };
    loc.strsize = compute_loc_size(&loc, false);

    if (caller != NULL) {
        memcpy(&loc.call_stack[0], &caller->call_stack[0], sizeof(struct _source_location) * caller->call_stack_size);
        loc.call_stack_size = caller->call_stack_size;
        loc.callers_str_cumulative_size = caller->callers_str_cumulative_size;
        if (caller->call_stack_size == SOURCE_LOCATION_STACK_MAX) {
            LOG("[WARNING] max call stack size hit ! (%d)\n[WARNING]Erasing oldest caller !", SOURCE_LOCATION_STACK_MAX);
            loc.callers_str_cumulative_size -= _compute_loc_size(&loc.call_stack[0], true);
            memmove(&loc.call_stack[0], &loc.call_stack[1], sizeof(struct _source_location) * (SOURCE_LOCATION_STACK_MAX - 1));
            loc.call_stack_size--;
        }

        loc.call_stack[loc.call_stack_size++] = *(const struct _source_location*)caller;
        loc.callers_str_cumulative_size += compute_loc_size(caller, true);
    }
    return loc;
}

void loc_exit(void) {
    free(_loc_str_ptr);
    free(loc_str_ptr);
}
