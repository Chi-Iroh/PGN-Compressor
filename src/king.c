#include <stdlib.h>
#include <string.h>

#include "../include/common.h"
#include "../include/coord_constants.h"
#include "../include/error.h"
#include "../include/king.h"
#include "../include/piece.h"

bool can_king_move_to(struct coord from, struct coord to, enum player moving_player, board board, bool check_is_is_dest_square_safe) {
    if (abs(from.file - to.file) > 1 || abs(from.rank - to.rank) > 1) {
        return false; // king can only move 1 square
    } else if (board_at_coord(board, to)->type != EMPTY_SQUARE) {
        return false;
    }

    if (check_is_is_dest_square_safe) {
        return !is_square_attacked_by(board, to, opponent_player(moving_player), false);
    }
    return true;
}

struct coord king_starting_coords[PLAYER_SIZE] = {
    [WHITE] = MAKE_CONSTANT_COORD(E,1),
    [BLACK] = MAKE_CONSTANT_COORD(E,8)
};

struct coord king_ending_coords[PLAYER_SIZE][CASTLING_SIZE] = {
    [WHITE] = {
        [KINGSIDE] = MAKE_CONSTANT_COORD(G,1),
        [QUEENSIDE] = MAKE_CONSTANT_COORD(C,1)
    },
    [BLACK] = {
        [KINGSIDE] = MAKE_CONSTANT_COORD(G,8),
        [QUEENSIDE] = MAKE_CONSTANT_COORD(C,8)
    }
};

struct coord rook_starting_coords[PLAYER_SIZE][CASTLING_SIZE] = {
    [WHITE] = {
        [KINGSIDE] = MAKE_CONSTANT_COORD(H,1),
        [QUEENSIDE] = MAKE_CONSTANT_COORD(A,1)
    },
    [BLACK] = {
        [KINGSIDE] = MAKE_CONSTANT_COORD(H,8),
        [QUEENSIDE] = MAKE_CONSTANT_COORD(A,8)
    }
};

struct coord rook_ending_coords[PLAYER_SIZE][CASTLING_SIZE] = {
    [WHITE] = {
        [KINGSIDE] = MAKE_CONSTANT_COORD(F,1),
        [QUEENSIDE] = MAKE_CONSTANT_COORD(D,1)
    },
    [BLACK] = {
        [KINGSIDE] = MAKE_CONSTANT_COORD(F,8),
        [QUEENSIDE] = MAKE_CONSTANT_COORD(D,8)
    }
};

bool parse_castling_move(struct move* move, const char* str, enum player moving_player) {
    *move = (struct move) {
        .player = moving_player,
        .piece = KING
    };
    enum castling castling;

    if (strncmp(str, "O-O-O", 5) == 0 || strncmp(str, "0-0-0", 5)) { // castling may be represented with capital o or zeroes
        move->string_len = 5;
        castling = QUEENSIDE;
    } else if (strncmp(str, "O-O", 3) || strncmp(str, "0-0", 3)) { // castling may be represented with capital o or zeroes
        move->string_len = 3;
        castling = KINGSIDE;
    } else {
        return false;
    }

    move->extra_infos = (struct extra_infos) {
        .piece_type = KING,
        .infos = {
            .king_infos = (struct king_move_infos) {
                .castling = castling
            }
        }
    };
    move->from = king_starting_coords[moving_player];
    move->to = king_ending_coords[moving_player][castling];
    return true;
}

static bool is_square_crossed_by_king_during_castling(struct coord square, struct coord end, int file_increment) {
    return (file_increment == 1) ?
        square.file <= end.file :
        square.file >= end.file;
}

static bool check_castling_move(const struct move* move, board board) {
    ASSERT_PRINTF(move->extra_infos.piece_type == KING, "Move '%s' is not a castling move !", move->algebraic_move);
    const struct king_move_infos* const castling_infos = &move->extra_infos.infos.king_infos;
    const enum castling castling = castling_infos->castling;
    const int file_increment = castling == KINGSIDE ? 1 : -1;
    struct coord position = king_starting_coords[move->player];
    const struct coord end = king_ending_coords[move->player][castling];
    const enum player opponent = opponent_player(move->player);
    const struct coord rook_coords = rook_starting_coords[move->player][castling];

    const struct piece rook_coords_square = *board_at_coord(board, rook_coords);
    const struct piece current_coords_square = *board_at_coord(board, position);
    if (rook_coords_square.type != ROOK || rook_coords_square.player != move->player) {
        return false;
    } else if (current_coords_square.type != KING || current_coords_square.player != move->player) {
        return false;
    }

    position.file += file_increment;
    while (position.file != rook_coords.file) {
        if (board_at_coord(board, position)->type != EMPTY_SQUARE) {
            return false;
        } else if (is_square_crossed_by_king_during_castling(position, end, file_increment) && is_square_attacked_by(board, position, opponent, false)) {
            return false;
        }
        position.file += file_increment;
    }
    return true;
}

// forwards last argument
static bool _can_king_move_to(struct coord from, struct coord to, enum player moving_player, board board) {
    return can_king_move_to(from, to, moving_player, board, true);
}

bool parse_king_move(struct move* move, const char* str, enum player moving_player, board board) {
    if (parse_move(move, KING, str, moving_player)) {
        ASSERT_PRINTF(find_starting_square(board, move, _can_king_move_to), "Cannot find a starting square !\nMove: '%s'", move->algebraic_move);
        return true;
    } else if (parse_castling_move(move, str, moving_player)) {
        ASSERT_PRINTF(check_castling_move(move, board), "Forbidden castling !\nMove: %s", move->algebraic_move);
        return move;
    }
    return false;
}
