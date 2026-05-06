#include <stdlib.h>
#include <string.h>

#include "../include/common.h"
#include "../include/debug.h"
#include "../include/error.h"
#include "../include/log.h"
#include "../include/parse.h"
#include "../include/piece.h"
#include "../include/pawn.h"
#include "../include/test.h"

PRIVATE_FUNCTION bool _parse_pawn_move(struct move* move, const char* str, enum player moving_player) {
    const size_t len = strlen(str);
    size_t i = 0;
    *move = (struct move) { .player = moving_player };

    ASSERT_PRINTF(len >= 2, "Pawn move cannot be less than 2 chars, but got string with size %zu !", len); // min size is 2, i.e. e8 or d4
    move->extra_infos.infos.pawn_infos = EMPTY_PAWN_MOVE_INFOS;
    struct pawn_move_infos* pawn_infos = &move->extra_infos.infos.pawn_infos;
    move->capture = str[1] == 'x';
    if (move->capture) {
        ASSERT_PRINTF(len >= 4 && is_file(str[i]), "When capturing, minimum move length is 4 (got %zu) and next char must be a valid file (got %c) !", len, str[i]); // when capturing, min size is 4, i.e. exd5 or dxc4
        move->from.file = to_file(str[i++]);
        i++; // skipping capture 'x'
    }
    ASSERT_PRINTF(is_file(str[i]), "File was expected !");
    move->to.file = to_file(str[i++]);
    ASSERT_PRINTF(is_rank(str[i]), "Rank was expected !");
    move->to.rank = to_rank(str[i++]);
    if (!move->capture) {
        move->from.file = move->to.file; // if no capture, pawn moves forward
    }
    if (str[i] == '=') {
        i++;
        ASSERT_PRINTF(is_piece(str[i]), "Piece (not pawn) was expected !");
        pawn_infos->promoted = true;
        pawn_infos->promotion_piece = to_piece(str[i++]);
    }
    move->check = to_check(str[i]);
    if (move->check != NO_CHECK) {
        i++;
    }
    move->piece = PAWN;
    if (str[i]) {
        ASSERT_PRINTF(strcmp(str + i, " e.p.") == 0 && !pawn_infos->promoted, "En passant explicit notation 'e.p.' was expected but got '%s' instead !", str + i);
        pawn_infos->en_passant = true;
        i += 5;
    }
    move->string_len = i;
    return true;
}

void check_for_en_passant(struct move* move, struct board_state* state) {
    if (state->move_turn == 0 && state->current_player == WHITE) {
        return; // to en passant, the previous must be checked, so no en passant on the very first turn
    }

    move->extra_infos.infos.pawn_infos.en_passant = false;
    move->extra_infos.infos.pawn_infos.has_en_passant_extra_ep_notation = false;

    const int rank_diff = abs(move->from.rank - move->to.rank);
    const int previous_rank_diff = abs(state->previous_move.from.rank - state->previous_move.to.rank);

    if (previous_rank_diff == 2 && state->previous_move.to.rank == move->from.rank) {
        if (rank_diff == 1) {
            if (abs(move->from.file - state->previous_move.to.file) == 1) { // to en passant, pawns must be next to each other
                const int forward_increment = move->player == WHITE ? 1 : -1;
                if (state->previous_move.to.rank - forward_increment == move->to.rank) { // must go forwards
                    ASSERT_PRINTF_EXIT_PROGRAM(++state->en_passant.nth_en_passant > state->en_passant.n_en_passant, "More en passant in game than in the header !");
                    LOG("en passant detected");
                    move->capture = true;
                    move->extra_infos.infos.pawn_infos.en_passant = true;
                    move->extra_infos.infos.pawn_infos.has_en_passant_extra_ep_notation = state->en_passant.has_en_passant_extra_ep_notation[state->en_passant.nth_en_passant];
                }
            }
        }
    }
}

static bool can_pawn_en_passant_to(struct coord from, struct coord to, enum player moving_player, struct board_state* state) {
    printf("EN PASSANT CHECK, from %c%i to %c%i\n", 'A' + from.file, 1 + from.rank, 'A' + to.file, 1 + to.rank);
    const int forward = (moving_player == WHITE) ? 1 : -1;
    if (from.rank + forward != to.rank) { // an en passant is always a capture
        puts("1");
        return false;
    } if (abs(from.file - to.file) != 1) {
        puts("2");
        return false;
    }

    if (moving_player == WHITE) {
        if (from.rank != 4) {
            puts("3");
            return false; // White pawns can only en passant on the 5th rank
        }
    } else if (moving_player == BLACK) {
        if (from.rank != 3) {
            puts("4");
            return false; // Black pawns can only en passant on the 4th rank
        }
    }

    const enum player opponent = opponent_player(moving_player);
    const struct coord nearby_pawn = { .file = to.file, from.rank }; // the pawn to capture is on the same rank as the pawn we move, but on a different file
    const struct piece next_piece = state->board[nearby_pawn.rank][nearby_pawn.file];
    if (next_piece.type == PAWN && next_piece.player == opponent) {
        const struct piece* const to_piece = board_at_coord(state->board, to);
        if (to_piece->type == EMPTY_SQUARE) {
            const struct move* last_move = &state->previous_move;
            const int opponent_forward = (last_move->player == WHITE) ? 1 : -1;
            if (last_move->player != opponent) {
                puts("DEBUG bad move:");
                print_move(last_move, stdout);
                puts("5");
                return false; // Last move wasn't the opponent's
            } else if (last_move->piece != PAWN) {
                puts("6");
                return false; // No pawn moved last turn
            } else if (!are_coords_equal(&last_move->to, &nearby_pawn)) {
                puts("7");
                return false; // Cannot en passant anymore, that pawn didn't move the last turn
            } else if (!are_coords_equal(&last_move->from, &(struct coord){ .file = nearby_pawn.file, .rank = nearby_pawn.rank - 2 * opponent_forward })) {
                printf("Last move started from %c%i instead of mandatory %c%i\n", 'A' + last_move->from.file, 1 + last_move->from.rank, 'A' + nearby_pawn.file, 1 + nearby_pawn.rank - 2 * forward);
                puts("8");
                return false; // Cannot en passant anymore, that pawn didn't move 2 squares the last turn
            }
            return true;
        } else {
            puts("Dest square isn't empty !");
        }
    } else {
        puts("No enemy pawn nearby to capture with en passant !");
    }
    puts("9");
    return false;
}

bool can_pawn_move_to(struct coord from, struct coord to, enum player moving_player, struct board_state* state) {
    if (moving_player == WHITE && to.rank < from.rank) {
        return false; // moving backwards is forbidden
    } else if (moving_player == BLACK && to.rank > from.rank) {
        return false; // moving backwards is forbidden
    }

    if (can_pawn_en_passant_to(from, to, moving_player, state)) {
        state->previous_move.extra_infos.piece_type = PAWN;
        state->previous_move.extra_infos.infos.pawn_infos = (struct pawn_move_infos) {
            .en_passant = true,
            .has_en_passant_extra_ep_notation = false,
            .promoted = false,
            .promotion_piece = EMPTY_SQUARE
        };
        return true;
    }

    if (from.file == 4 && from.rank == 4) {
        puts("E5 Prev move can_pawn_move_to :");
        print_move(&state->previous_move, stdout);
    }

    const int rank_diff = abs(from.rank - to.rank);
    const int increment = to.rank > from.rank ? 1 : -1;
    if (from.file == to.file) {

        if (rank_diff > 2) {
            return false;
        }
        for (int i = 1; i <= rank_diff; i++) {
            if (board_at(state->board, from.file, from.rank + i * increment)->type != EMPTY_SQUARE) {
                return false;
            }
        }
        return true;
    } else { // capturing
        if (rank_diff != 1) {
            return false;
        } else if (abs(from.file - to.file) != 1) {
            return false; // when capturing, moves 1 file
        } else if (rank_diff != increment) {
            return false; // when capturing, moves 1 rank forward
        }
        if (board_at_coord(state->board, to)->type != EMPTY_SQUARE) {
            return board_at_coord(state->board, to)->player != moving_player;
        }
        return false;
    }
}

bool parse_pawn_move(struct move* move, const char* str, enum player moving_player, struct board_state* state) {
    if (!_parse_pawn_move(move, str, moving_player)) {
        return false;
    }

    ASSERT_PRINTF(find_starting_square(state, move, can_pawn_move_to), "Cannot find a starting square !\nMove: %s", move->algebraic_move);
    return true;
}
