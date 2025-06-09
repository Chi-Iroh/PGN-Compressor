#pragma once
#include "piece.h"

#define A_FILE 0
#define B_FILE 1
#define C_FILE 2
#define D_FILE 3
#define E_FILE 4
#define F_FILE 5
#define G_FILE 6
#define H_FILE 7

#define RANK_1 0
#define RANK_2 1
#define RANK_3 2
#define RANK_4 3
#define RANK_5 4
#define RANK_6 5
#define RANK_7 6
#define RANK_8 7

#define MAKE_CONSTANT_COORD(_file, _rank) { .file = _file##_FILE, .rank = RANK_##_rank }
