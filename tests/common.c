#include <stdio.h>

#include "../include/apply_move.h"
#include "./common.h"

enum token_type piece_to_token_type(enum piece_type piece) {
    switch (piece) {
        case KING: return MOVE_KING;
        case QUEEN: return MOVE_QUEEN;
        case BISHOP: return MOVE_BISHOP;
        case KNIGHT: return MOVE_KNIGHT;
        case ROOK: return MOVE_ROOK;
        case PAWN: return MOVE_PAWN;

        default:
            puts("Cannot convert piece type to pgn token type !");
            exit(1);
    }
}

struct pgn_token move_to_token(struct move* move) {
    const bool castling = move->piece == KING && move->extra_infos.piece_type == KING && move->extra_infos.infos.king_infos.is_castling;
    return (struct pgn_token) {
        .type = castling ? CASTLING : piece_to_token_type(move->piece),
        .move = {
            .move = *move
        }
    };
}

void play_move(struct board_state* state, struct move* move) {
    const struct pgn_token pgn_token = move_to_token(move);
    apply_move(&pgn_token, state->board);
    state->previous_move = *move;
    next_turn(state);
}
