#include "../include/error.h"
#include "../include/piece.h"

#include "../include/bishop.h"
#include "../include/king.h"
#include "../include/knight.h"
#include "../include/pawn.h"
#include "../include/queen.h"
#include "../include/rook.h"

struct board_state empty_board_state(void) {
    struct board_state state = {
        .current_player = WHITE,
        .move_turn = 1
    };
    const enum piece_type FIRST_ROW_PIECES[BOARD_SIZE] = {
        ROOK, KNIGHT, BISHOP, QUEEN, KING, BISHOP, KNIGHT, ROOK
    };

    for (uint8_t rank = 0; rank < BOARD_SIZE; rank++) {
        const enum player player = rank <= 1 ? WHITE : BLACK;
        for (uint8_t file = 0; file < BOARD_SIZE; file++) {
            if (rank == 0 || rank == BOARD_SIZE - 1) {
                state.board[rank][file] = (struct piece) {
                    .type = FIRST_ROW_PIECES[file],
                    .player = player
                };
            } else if (rank == 1 || rank == BOARD_SIZE - 2) {
                state.board[rank][file] = (struct piece) {
                    .type = PAWN,
                    .player = player
                };
            } else {
                state.board[rank][file] = (struct piece) {
                    .type = EMPTY_SQUARE,
                    .player = INVALID_PLAYER
                };
            }
        }
    }
    return state;
}

void next_turn(struct board_state* state) {
    if (state != NULL) {
        if (state->current_player == BLACK) {
            state->current_player = WHITE;
            state->move_turn++;
        } else {
            state->current_player = BLACK;
        }
    }
}

const char* PLAYER_NAMES[PLAYER_SIZE] = {
    [WHITE] = "white",
    [BLACK] = "black",
    [INVALID_PLAYER] = "<invalid player>"
};

// static void find_king(board board, enum player player, struct coord* coord) {
//     ASSERT_PRINTF_RETURN(player != INVALID_PLAYER, "Invalid player !");
//     const struct piece king = {
//         .player = player,
//         .type = KING
//     };
//     for (unsigned rank = 0; rank < BOARD_SIZE; rank++) {
//         for (unsigned file = 0; file < BOARD_SIZE; file++) {
//             if (are_pieces_equal(board_at(board, file, rank), &king)) {
//                 *coord = (struct coord) {
//                     .file = file,
//                     .rank = rank
//                 };
//                 return;
//             }
//         }
//     }
//     FAIL("Cannot find %s king on the board !", PLAYER_NAMES[player]);
// }

// checks if player is in check
static bool is_in_check(board board, enum player player, struct coord* checking_piece_coord) {
    ASSERT_PRINTF(player != INVALID_PLAYER, "Invalid player !");

    const struct coord king_coord = find_king(board, player);
    const enum player opponent = opponent_player(player);
    for (int rank = 0; rank < BOARD_SIZE; rank++) {
        for (int file = 0; file < BOARD_SIZE; file++) {
            struct piece* const piece = board_at(board, file, rank);
            if (piece->player == opponent) {
                const struct coord from = {
                    .file = file,
                    .rank = rank
                };
                if (can_move_to[piece->type](from, king_coord, opponent, board)) {
                    *checking_piece_coord = from;
                    return true;
                }
            }
        }
    }
    return false;
}

// bool apply_move(struct board_state* state, const struct move* move) {
//     ASSERT_PRINTF(state != NULL, "Board state is NULL !");
//     ASSERT_PRINTF(move != NULL, "Move is NULL !");
//     ASSERT_PRINTF(state->current_player == move->player, "Bad player (%s instead of %s) !", PLAYER_NAMES[move->player], PLAYER_NAMES[state->current_player]);

//     if (!can_move_to[move->player](move->from, move->to, move->player, state->board)) {
//         return false;
//     }

//     struct piece* const from = board_at_coord(state->board, move->from);
//     struct piece* const to = board_at_coord(state->board, move->to);
//     const struct piece copy_to = *to;
//     *to = *from;
//     *from = (struct piece) {
//         .player = INVALID_PLAYER,
//         .type = EMPTY_SQUARE
//     };

//     struct coord checking_piece_coord;
//     if (is_in_check(state->board, move->player, &checking_piece_coord)) {
//         *from = *to; // moving back the piece to its starting square
//         *to = copy_to;
//         return false;
//     }

//     state->current_player = opponent_player(state->move_turn);
//     if (state->current_player == WHITE) {
//         state->move_turn++;
//     }
//     return true;
// }


