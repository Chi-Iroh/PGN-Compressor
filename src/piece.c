#include <string.h>
#include "../include/error.h"
#include "../include/piece.h"

const char* PIECES_NAME[] = {
    [KING] = "king",
    [QUEEN] = "queen",
    [BISHOP] = "bishop",
    [KNIGHT] = "knight",
    [ROOK] = "rook",
    [PAWN] = "pawn",
    [EMPTY_SQUARE] = "empty square"
};

const struct coord INVALID_COORD_STRUCT = {
    .file = INVALID_COORD,
    .rank = INVALID_COORD,
};

const struct pawn_move_infos EMPTY_PAWN_MOVE_INFOS = {
    .en_passant = false,
    .promoted = false,
    .promotion_piece = EMPTY_SQUARE
};

const struct move INVALID_MOVE = {
    .player = INVALID_PLAYER,
    .piece = EMPTY_SQUARE,
    .from = INVALID_COORD_STRUCT,
    .to = INVALID_COORD_STRUCT,
    .capture = false,
    .check = NO_CHECK,
    .extra_infos = {
        .piece_type = EMPTY_SQUARE
    },
    .string_len = 0,
    .algebraic_move = NULL
};

const enum piece_type PROMOTION_PIECE[4] = {
    [_0b00] = QUEEN,
    [_0b01] = BISHOP,
    [_0b10] = KNIGHT,
    [_0b11] = ROOK
};

enum player opponent_player(enum player player) {
    ASSERT_PRINTF_EXIT_PROGRAM(player < PLAYER_SIZE, "Unknown player (code %d), cannot determine its opponent !", player);
    return (player == WHITE) ? BLACK : WHITE;
}

bool are_coords_equal(const struct coord* first, const struct coord* second) {
    ASSERT_PRINTF_EXIT_PROGRAM(first != NULL, "First object is NULL !");
    ASSERT_PRINTF_EXIT_PROGRAM(second != NULL, "Second object is NULL !");
    return memcmp(first, second, sizeof(struct coord)) == 0;
}
