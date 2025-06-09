#include "../include/safe_bool.h"

enum safe_bool to_safe_bool(bool b) {
    return b ? TRUE : FALSE;
}

enum safe_bool safe_bool_and(enum safe_bool a, enum safe_bool b) {
    switch (a) {
        case ERROR: return ERROR;
        case TRUE: return to_safe_bool(b == TRUE);
        case FALSE: return FALSE;
    }
    return ERROR;
}

enum safe_bool safe_bool_or(enum safe_bool a, enum safe_bool b) {
    switch (a) {
        case ERROR: return ERROR;
        case TRUE: return TRUE;
        case FALSE: return to_safe_bool(b == TRUE);
    }
    return ERROR;
}
