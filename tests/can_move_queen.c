#include <criterion/criterion.h>

#include "../include/coord_constants.h"
#include "../include/debug.h"
#include "../include/queen.h"
#include "./move.h"

Test(can_queen_move, can_move_or_capture) {
    struct board_state state = empty_board_state();
    char expected[BOARD_SIZE][BOARD_SIZE + 1] = {
    //   ABCDEFGH
        "RNBQKBNR", // 1, WHITE
        "PPPP PPP", // 2
        "        ", // 3
        "        ", // 4
        "   P    ", // 5
        "        ", // 6
        "ppp pppp", // 7
        "rnbqkbnr"  // 8, BLACK
    //   ABCDEFGH
    };

    play_move(&state, &(struct move){ .from = E2, .to = E4, .player = WHITE, .piece = PAWN, .capture = false, .check = NO_CHECK });
    play_move(&state, &(struct move){ .from = D7, .to = D5, .player = BLACK, .piece = PAWN, .capture = false, .check = NO_CHECK });
    play_move(&state, &(struct move){ .from = E4, .to = D5, .player = WHITE, .piece = PAWN, .capture = true, .check = NO_CHECK });

    cr_assert(can_queen_move_to(D8, D5, BLACK, &state));
    free_board_state(&state);
}
