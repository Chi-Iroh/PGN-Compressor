#pragma once

#include "../include/piece.h"

void play_move(struct board_state* state, struct move* move);

#define ASSERT_BOARDS_ARE_EQUAL(board1, board2)                                 \
for (int rank = 0; rank < 8; rank++) {                                          \
    for (int file = 0; file < 8; file++) {                                      \
        struct piece* const piece1 = board_at((board1), file, rank);            \
        struct piece* const piece2 = board_at((board2), file, rank);            \
        cr_assert(                                                              \
            are_pieces_equal(piece1, piece2),                                   \
            "Different pieces at %c%c: '%s' has %s %s and '%s' has %s %s !",    \
            'a' + file, '1' + rank,                                             \
            #board1, PLAYER_NAMES[piece1->player], PIECES_NAME[piece1->type],   \
            #board2, PLAYER_NAMES[piece2->player], PIECES_NAME[piece2->type]    \
        );                                                                      \
    }                                                                           \
}
