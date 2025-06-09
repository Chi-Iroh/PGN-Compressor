#pragma once
#include <stdbool.h>

struct args {
    bool compress;
    bool uncompress;
    bool help;
    const char* input;
    const char* output;
};
