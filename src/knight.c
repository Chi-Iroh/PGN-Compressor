#include <stdlib.h>
#include "../include/common.h"
#include "../include/error.h"
#include "../include/knight.h"
#include "../include/parse.h"

bool can_knight_move_to(struct coord from, struct coord to, enum player moving_player, board board) {
    const int file_diff = abs(from.file - to.file);
    const int rank_diff = abs(from.rank - to.rank);

    if ((file_diff == 2 && rank_diff == 1) || (file_diff == 1 && rank_diff == 2)) {
        if (board_at_coord(board, to)->type != EMPTY_SQUARE) {
            return board_at_coord(board, to)->player != moving_player;
        }
        return true;
    }
    return false;
}

bool parse_knight_move(struct move* move, const char* str, enum player moving_player, board board) {
    if (!parse_move(move, KNIGHT, str, moving_player)) {
        return false;
    }

    ASSERT_PRINTF(find_starting_square(board, move, can_knight_move_to), "Cannot find a starting square !\nMove: %s", move->algebraic_move);
    return move;
}
