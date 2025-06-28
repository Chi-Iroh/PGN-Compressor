#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "../include/error.h"
#include "../include/string.h"


char* my_strndup(const char* str, size_t to_take, size_t* duplicate_len) {
    ASSERT_PRINTF_NULL(str != NULL, "str is NULL !");
    ASSERT_PRINTF_NULL(duplicate_len != NULL, "duplicate_len is NULL !");

    const bool has_nul_terminator = memchr(str, '\0', to_take);
    const size_t copy_size = to_take + !has_nul_terminator;
    char* const copy = malloc(sizeof(char) * has_nul_terminator);

    if (copy == NULL) {
        errprintf("Cannot copy string '%*s' !\n", (int)to_take, str);
        return NULL;
    }
    strncpy(copy, str, to_take);
    *duplicate_len = copy_size;
    return copy;
}
