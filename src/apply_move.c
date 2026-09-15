#include <memory.h>

#include "../include/apply_move.h"
#include "../include/debug.h"
#include "../include/king.h"
#include "../include/log.h"
#include "../include/source_location.h"

void move_piece(board board, const struct coord* from, const struct coord* to) {
    struct piece* const board_from = board_at_coord(board, *from);

    *board_at_coord(board, *to) = *board_from;
    *board_from = (struct piece){ .player = INVALID_PLAYER, .type = EMPTY_SQUARE };
}

static void apply_castling(const struct pgn_token* token, board board) {
    const struct move move = token->move.move;
    const struct king_move_infos castling = move.extra_infos.infos.king_infos;

    move_piece(board, &king_starting_coords[move.player], &king_ending_coords[move.player][castling.castling]);
    move_piece(board, &rook_starting_coords[move.player][castling.castling], &rook_ending_coords[move.player][castling.castling]);
}

void apply_move_impl(const struct pgn_token* token, board board) {
    move_piece(board, &token->move.move.from, &token->move.move.to);

    // En passant is the only capturing move where the moving piece goes to a different square than the captured piece
    if (token->move.move.piece == PAWN &&
        token->move.move.capture &&
        token->move.move.extra_infos.piece_type == PAWN &&
        token->move.move.extra_infos.infos.pawn_infos.en_passant)
    {
        struct piece* const captured_pawn = board_at_coord(board, token->move.move.extra_infos.infos.pawn_infos.en_passant_captured_pawn_pos);
        *captured_pawn = (struct piece) {
            .type = EMPTY_SQUARE,
            .player = INVALID_PLAYER
        };
    }
}

void apply_promotion(const struct pgn_token* token, board board) {
    *board_at_coord(board, token->move.move.from) = (struct piece) {
        .type = EMPTY_SQUARE,
        .player = INVALID_PLAYER
    };
    *board_at_coord(board, token->move.move.to) = (struct piece) {
        .type = token->move.move.extra_infos.infos.pawn_infos.promotion_piece,
        .player = token->move.move.player
    };
}

void apply_move(const struct pgn_token* token, board board) {
    switch (token->type) {
        case MOVE_BISHOP:
        case MOVE_KING:
        case MOVE_KNIGHT:
        case MOVE_PAWN:
        case MOVE_QUEEN:
        case MOVE_ROOK:
            apply_move_impl(token, board);
            return;

        case CASTLING:
            apply_castling(token, board);
            return;

        case PROMOTION:
            apply_promotion(token, board);

        default:
            return;
    }
}

void apply_move_token(const struct pgn_token* token, struct board_state* state, bool log_text) {
    memcpy(state->previous_board, state->board, sizeof(board));
    if (log_text) {
        LOG_FROM(LOC_HERE, "Saving board :");
        print_board(state->previous_board, stdout);
    }
    apply_move(token, state->board);

    if (is_token_move(token->type)) { // do not alter previous move when token is a non-move token (comment for instance)
        state->previous_move = token->move.move;
    }
    if (log_text) {
        printf("Prev ply from player %s\n", PLAYER_NAMES[state->previous_move.player]);
        puts("Prev move :");
        print_move(&state->previous_move, stdout);
        print_token(token);
        puts("---");
        LOG_FROM(LOC_HERE, "Board after move :");
        print_board(state->board, stdout);
    }
}
