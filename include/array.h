#pragma once
#include <stdbool.h>
#include <stddef.h>

/**
 * If size_threshold is NULL, then NULL is returned.
 * If n_elems == *size_threshold, then arr_ptr is reallocated to hold n_elems * capacity_multiplier elems, and *size_threshold is updated to hold the new size.
 * If it fails, NULL is returned and arr_ptr is untouched.
 * If either arr_ptr or arr_ptr is NULL, then nothing is done and NULL is returned.
 * If n_elems > size_threshold, then nothing is done and NULL is returned.
 */
void* expand_array_if_needed(void* arr, size_t n_elems, size_t elem_size, size_t* size_threshold, size_t capacity_multiplier);
