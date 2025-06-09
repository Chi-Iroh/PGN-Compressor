#include "../include/common.h"
#include "../include/error.h"
#include "../include/rook.h"
#include "../include/parse.h"

bool can_rook_move_to(struct coord from, struct coord to, enum player moving_player, board board) {
    const bool same_rank = from.rank == to.rank;
    const bool same_file = from.file == to.file;

    if (!same_rank && !same_file) {
        return false; // rooks move in straight lines
    }
    const int increment = same_file     ?
        (from.rank < to.rank ? 1 : -1)  :
        (from.file < to.file ? 1 : -1);
    int* start = same_rank ? &from.file : &from.rank;
    const int end = same_rank ? to.file : to.rank;

    while (*start != end) {
        if (board_at_coord(board, from)->type != EMPTY_SQUARE) {
            return false; // rooks cannot jump over pieces
        }
        start += increment;
    }
    if (board_at_coord(board, to)->type != EMPTY_SQUARE) {
        return board_at_coord(board, to)->player != moving_player;
    }
    return true;
}

bool parse_rook_move(struct move* move, const char* str, enum player moving_player, board board) {
    if (!parse_move(move, ROOK, str, moving_player)) {
        return false;
    }

    ASSERT_PRINTF(find_starting_square(board, move, can_rook_move_to), "Cannot find a starting square !\nMove: %s", move->algebraic_move);
    return true;
}
