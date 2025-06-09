#pragma once
#include <stdbool.h>

enum safe_bool {
    ERROR = -1,
    FALSE = 0,
    TRUE = 1
};

enum safe_bool to_safe_bool(bool b);
enum safe_bool safe_bool_and(enum safe_bool a, enum safe_bool b);
enum safe_bool safe_bool_or(enum safe_bool a, enum safe_bool b);
