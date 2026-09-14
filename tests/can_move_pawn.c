#include <criterion/criterion.h>

#include "../include/coord_constants.h"
#include "../include/debug.h"
#include "../include/pawn.h"
#include "../include/strings.h"
#include "./move.h"

Test(can_pawn_move, can_move_or_capture) {
    struct board_state state = empty_board_state();
    char expected[BOARD_SIZE][BOARD_SIZE + 1] = {
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
    board expected_board;
    read_board(expected_board, expected);

    play_move(&state, &(struct move) { .from = G2, .to = G4, .piece = PAWN, .player = WHITE, .capture = false, .check = NO_CHECK }); // this pawn is pushed to test regular capture
    play_move(&state, &(struct move) { .from = A7, .to = A6, .piece = PAWN, .player = BLACK, .capture = false, .check = NO_CHECK }); // dummy move
    play_move(&state, &(struct move) { .from = E2, .to = E4, .piece = PAWN, .player = WHITE, .capture = false, .check = NO_CHECK });
    play_move(&state, &(struct move) { .from = H7, .to = H6, .piece = PAWN, .player = BLACK, .capture = false, .check = NO_CHECK }); // dummy move before pushing the F pawn
    play_move(&state, &(struct move) { .from = E4, .to = E5, .piece = PAWN, .player = WHITE, .capture = false, .check = NO_CHECK });
    play_move(&state, &(struct move) { .from = F7, .to = F5, .piece = PAWN, .player = BLACK, .capture = false, .check = NO_CHECK }); // now white can en passant

    cr_assert(can_pawn_move_to(E5, E6, WHITE, &state) && "simply move forward");
    cr_assert(can_pawn_move_to(G4, F5, WHITE, &state) && "regular capture");
    cr_assert(can_pawn_move_to(E5, F6, WHITE, &state) && "en passant");
    free_board_state(&state);
}

Test(can_pawn_move, en_passant) {
    struct board_state state = empty_board_state();
    play_move(&state, &(struct move){ .from = E2, .to = E4, .piece = PAWN, .player = WHITE, .capture = false, .check = NO_CHECK });
    play_move(&state, &(struct move){ .from = E7, .to = E6, .piece = PAWN, .player = BLACK, .capture = false, .check = NO_CHECK });
    play_move(&state, &(struct move){ .from = E4, .to = E5, .piece = PAWN, .player = WHITE, .capture = false, .check = NO_CHECK });
    play_move(&state, &(struct move){ .from = D7, .to = D5, .piece = PAWN, .player = BLACK, .capture = false, .check = NO_CHECK });
    play_move(&state, &(struct move){ .from = E5, .to = D6, .piece = PAWN, .player = WHITE, .capture = true, .check = NO_CHECK, .extra_infos = { .piece_type = PAWN, .infos = { .pawn_infos = { .en_passant = true, .has_en_passant_extra_ep_notation = false, .promoted = false, .promotion_piece = EMPTY_SQUARE, .en_passant_captured_pawn_pos = D5 } } } });

    char expected[BOARD_SIZE][BOARD_SIZE + 1] = {
    //   ABCDEFGH
        "RNBQKBNR", // 1, WHITE
        "PPPP PPP", // 2
        "        ", // 3
        "        ", // 4
        "        ", // 5
        "   Pp   ", // 6
        "ppp  ppp", // 7
        "rnbqkbnr"  // 8, BLACK
    //   ABCDEFGH
    };
    board expected_board = { 0 };
    read_board(expected_board, expected);

    const int d = memcmp(expected_board, state.board, sizeof(board));
    if (d != 0) {
        printf("%d\n", d);
        puts("Got :");
        print_board(state.board, stderr);
        puts("But expected :");
        print_board(expected_board, stderr);
        cr_assert(false);
    }
}
