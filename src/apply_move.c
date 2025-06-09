#include "../include/apply_move.h"
#include "../include/king.h"

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

        default:
            return;
    }
}
