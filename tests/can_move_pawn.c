#include <criterion/criterion.h>
#include "../include/coord_constants.h"
#include "../include/debug.h"
#include "../include/pawn.h"
#include "../include/piece.h"

Test(can_pawn_move, forward) {
    struct board_state state = empty_board_state();
    char board[BOARD_SIZE][BOARD_SIZE + 1] = {
    //   ABCDEFGH
        "RNBQKBNR", // 1, WHITE
        "PPPP P P", // 2
        "        ", // 3
        "      P ", // 4
        "    Pp  ", // 5
        "p      p", // 6
        " pppp p ", // 7
        "rnbqkbnr"  // 8, BLACK
    //   ABCDEFGH
    };

    puts("--- WHITE TO PLAY ---");
    apply_move(&state, &(struct move) { .from = G2, .to = G4, .piece = PAWN, .player = WHITE, .capture = false, .check = NO_CHECK }); // this pawn is pushed to test regular capture
    puts("--- BLACK TO PLAY ---");
    apply_move(&state, &(struct move) { .from = A7, .to = A6, .piece = PAWN, .player = BLACK, .capture = false, .check = NO_CHECK }); // dummy move

    puts("--- WHITE TO PLAY ---");
    apply_move(&state, &(struct move) { .from = E2, .to = E4, .piece = PAWN, .player = WHITE, .capture = false, .check = NO_CHECK });
    puts("--- BLACK TO PLAY ---");
    apply_move(&state, &(struct move) { .from = H7, .to = H6, .piece = PAWN, .player = BLACK, .capture = false, .check = NO_CHECK }); // dummy move before pushing the F pawn
    puts("--- WHITE TO PLAY ---");
    apply_move(&state, &(struct move) { .from = E4, .to = E5, .piece = PAWN, .player = WHITE, .capture = false, .check = NO_CHECK });
    puts("--- BLACK TO PLAY ---");
    apply_move(&state, &(struct move) { .from = F7, .to = F5, .piece = PAWN, .player = BLACK, .capture = false, .check = NO_CHECK }); // now white can en passant

    puts("");
    puts("BOARD here :");
    print_board(state.board);

    cr_assert(can_pawn_move_to(E5, E6, WHITE, &state) && "simply move forward");
    cr_assert(can_pawn_move_to(G4, F5, WHITE, &state) && "regular capture");

    puts("\n---------------------\n");
    cr_assert(can_pawn_move_to(E5, F6, WHITE, &state) && "en passant");
    free_board_state(&state);
}
