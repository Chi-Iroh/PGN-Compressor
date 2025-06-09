#include <stdlib.h>
#include "../include/bishop.h"
#include "../include/common.h"
#include "../include/error.h"
#include "../include/parse.h"

bool can_bishop_move_to(struct coord from, struct coord to, enum player moving_player, board board) {
    const int file_diff = to.file - from.file;
    const int rank_diff = to.rank - from.rank;

    if (abs(file_diff) != abs(rank_diff)) {
        return false;
    }
    const int file_increment = (file_diff > 0) ? 1 : -1;
    const int rank_increment = (rank_diff > 0) ? 1 : -1;

    for (int distance = abs(file_diff); distance > 0; distance--) {
        from.file += file_increment;
        from.rank += rank_increment;
        if (distance != 1 && board_at_coord(board, from)->type != EMPTY_SQUARE) { // bishop cannot jump over pieces
            return false;
        }
    }
    if (board_at_coord(board, to)->type != EMPTY_SQUARE) {
        return board_at_coord(board, to)->player != moving_player;
    }
    return true;
}

bool parse_bishop_move(struct move* move, const char* str, enum player moving_player, board board) {
    if (!parse_move(move, BISHOP, str, moving_player)) {
        return false;
    }

    ASSERT_PRINTF(find_starting_square(board, move, can_bishop_move_to), "Cannot find a starting square !\nMove: %s", move->algebraic_move);
    return move;
}
