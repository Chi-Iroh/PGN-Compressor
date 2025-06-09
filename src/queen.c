#include <stdlib.h>
#include "../include/bishop.h"
#include "../include/common.h"
#include "../include/error.h"
#include "../include/parse.h"
#include "../include/queen.h"
#include "../include/rook.h"

bool can_queen_move_to(struct coord from, struct coord to, enum player moving_player, board board) {
    return can_bishop_move_to(from, to, moving_player, board) || can_rook_move_to(from, to, moving_player, board);
}

bool parse_queen_move(struct move* move, const char* str, enum player moving_player, board board) {
    if (!parse_move(move, QUEEN, str, moving_player)) {
        return false;
    }

    ASSERT_PRINTF(find_starting_square(board, move, can_queen_move_to), "Cannot find a starting square !\nMove: %s", move->algebraic_move);
    return move;
}
