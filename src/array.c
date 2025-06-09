#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "../include/array.h"
#include "../include/error.h"

void* expand_array_if_needed(void* arr, size_t n_elems, size_t elem_size, size_t* size_threshold, size_t capacity_multiplier) {
    ASSERT_PRINTF_NULL(size_threshold != NULL, "size threshold is NULL !");

    if (arr == NULL) {
        return NULL;
    } else if (n_elems < *size_threshold) {
        return arr;
    } else if (n_elems > *size_threshold) {
        fprintf(stderr, "size > expansion threshold !");
        return NULL;
    }

    const size_t new_size = *size_threshold * capacity_multiplier;
    void* const expanded_arr = realloc(arr, new_size * elem_size);

    if (expanded_arr == NULL) {
        errprintf("Error while reallocating %zu bytes !\n", new_size * elem_size);
        return NULL;
    }
    memset((uint8_t*)expanded_arr + n_elems, 0, elem_size * (new_size - n_elems));
    *size_threshold = new_size;
    return expanded_arr;
}
