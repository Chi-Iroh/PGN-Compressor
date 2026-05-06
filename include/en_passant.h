#pragma once

#include <stdbool.h>
#include <stdint.h>

#define MAX_EN_PASSANT 8
#define N_EN_PASSANT_BITS 4 // 0 min to 8 en passant max, thus 9 possibilities = 4 bits

struct en_passant {
    uint8_t n_en_passant : N_EN_PASSANT_BITS;
    uint8_t nth_en_passant;
    bool has_en_passant_extra_ep_notation[MAX_EN_PASSANT];
};
