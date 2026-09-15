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
    .en_passant_captured_pawn_pos = INVALID_COORD_STRUCT,
    .has_en_passant_extra_ep_notation = false,
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
    ASSERT_PRINTF_EXIT_PROGRAM(player != INVALID_PLAYER, "Cannot determine the opponent of an invalid player !");
    ASSERT_PRINTF_EXIT_PROGRAM(player < PLAYER_SIZE, "Unknown player (code %d), cannot determine its opponent !", player);
    return (player == WHITE) ? BLACK : WHITE;
}

bool are_coords_equal(const struct coord* first, const struct coord* second) {
    ASSERT_PRINTF_EXIT_PROGRAM(first != NULL, "First object is NULL !");
    ASSERT_PRINTF_EXIT_PROGRAM(second != NULL, "Second object is NULL !");
    return memcmp(first, second, sizeof(struct coord)) == 0;
}

bool is_token_move(enum token_type type) {
    switch (type) {
        case MOVE_KING:
        case MOVE_QUEEN:
        case MOVE_BISHOP:
        case MOVE_KNIGHT:
        case MOVE_ROOK:
        case MOVE_PAWN:
        case CASTLING:
        case PROMOTION:
        case ALTERNATIVE_MOVE:
            return true;

        default:
            return false;
    }
}
