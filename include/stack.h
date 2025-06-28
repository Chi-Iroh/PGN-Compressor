#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define STACK_STRUCT_WITH_NAME(type, name)  \
struct stack_##name {                       \
    size_t capacity;                        \
    size_t size;                            \
    type* stack;                            \
};
#define STACK_STRUCT(type) STACK_STRUCT_WITH_NAME(type, type)

#define STACK_PROTOTYPES_WITH_NAME(type, name, empty_value)             \
struct stack_##name stack_##name##_empty(void);                         \
bool stack_##name##_is_empty(struct stack_##name* stack);               \
bool stack_##name##_push(struct stack_##name* stack, type elem);        \
bool stack_##name##_pop(struct stack_##name* stack, type* popped_elem); \
void stack_##name##_free(struct stack_##name* stack);
#define STACK_PROTOTYPES(type, empty_value) STACK_PROTOTYPES_WITH_NAME(type, type, empty_value)

#define STACK_IMPL_WITH_NAME(type, name, empty_value)                                 \
struct stack_##name stack_##name##_empty(void) {                                        \
    struct stack_##name empty = {                                                       \
        .capacity = 0,                                                                  \
        .size = 0,                                                                      \
        .stack = NULL                                                                   \
    };                                                                                  \
    return empty;                                                                       \
}                                                                                       \
                                                                                        \
bool stack_##name##_is_empty(struct stack_##name* stack) {                              \
    return stack->size == 0;                                                            \
}                                                                                       \
                                                                                        \
bool stack_##name##_push(struct stack_##name* stack, type elem) {                       \
    if (stack->stack == NULL) {                                                         \
        stack->stack = malloc(sizeof(type));                                            \
        if (stack->stack == NULL) {                                                     \
            return false;                                                               \
        }                                                                               \
        stack->capacity = 1;                                                            \
    } else if (stack->size == stack->capacity) {                                        \
        type* const new_stack = malloc(sizeof(type) * stack->capacity * 2);             \
        if (new_stack == NULL) {                                                        \
            free(stack->stack);                                                         \
            return false;                                                               \
        }                                                                               \
        stack->capacity *= 2;                                                           \
    }                                                                                   \
    stack->stack[stack->size++] = elem;                                                 \
    return true;                                                                        \
}                                                                                       \
                                                                                        \
bool stack_##name##_pop(struct stack_##name* stack, type* popped_elem) {                \
    if (stack->size == 0) {                                                             \
        return false;                                                                   \
    }                                                                                   \
    *popped_elem = stack->stack[--stack->size];                                         \
    return true;                                                                        \
}                                                                                       \
                                                                                        \
void stack_##name##_free(struct stack_##name* stack) {                                  \
    free(stack->stack);                                                                 \
}
#define STACK_IMPL(type, empty_value) STACK_IMPL_WITH_NAME(type, type, empty_value)
