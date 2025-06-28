#include <criterion/criterion.h>
#include "../include/stack.h"

STACK_STRUCT(int)
STACK_IMPL(int, 0)

Test(stack_int, empty) {
    struct stack_int stack = stack_int_empty();

    cr_assert(stack_int_is_empty(&stack));
    stack_int_free(&stack);
}

Test(stack_int, push_pop) {
    struct stack_int stack = stack_int_empty();

    stack_int_push(&stack, 71);
    cr_assert(stack.size == 1);

    int n;
    cr_assert(stack_int_pop(&stack, &n));
    cr_assert(n == 71);
    cr_assert(stack.size == 0);

    stack_int_free(&stack);
}

struct object {
    int n;
    const char* str;
    float f;
};

STACK_STRUCT_WITH_NAME(struct object, object)
STACK_IMPL_WITH_NAME(struct object, object, ({ .n = 0, .str = NULL, .f = 0.0f }))

Test(stack_object, empty) {
    struct stack_object stack = stack_object_empty();

    cr_assert(stack_object_is_empty(&stack));
    stack_object_free(&stack);
}

Test(stack_object, push_pop) {
    struct stack_object stack = stack_object_empty();

    const char* str = "helloworld";
    stack_object_push(&stack, (struct object){ .f = 5.1f, .n = 456, .str = str });
    cr_assert(stack.size == 1);

    struct object n;
    cr_assert(stack_object_pop(&stack, &n));
    cr_assert(n.f == 5.1f);
    cr_assert(n.n == 456);
    cr_assert(n.str = str);
    cr_assert(stack.size == 0);

    stack_object_free(&stack);
}
