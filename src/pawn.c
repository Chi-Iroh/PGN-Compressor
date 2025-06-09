#include <stdlib.h>
#include <string.h>

#include "../include/common.h"
#include "../include/error.h"
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

bool can_pawn_move_to(struct coord from, struct coord to, enum player moving_player, board board) {
    if (moving_player == WHITE && to.rank < from.rank) {
        return false; // moving backwards is forbidden
    } else if (moving_player == BLACK && to.rank > from.rank) {
        return false; // moving backwards is forbidden
    }

    const int rank_diff = abs(from.rank - to.rank);
    const int increment = to.rank > from.rank ? 1 : -1;
    if (from.file == to.file) {

        if (rank_diff > 2) {
            return false;
        }
        for (int i = 1; i <= rank_diff; i++) {
            if (board_at(board, from.file, from.rank + i * increment)->type != EMPTY_SQUARE) {
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
        if (board_at_coord(board, to)->type != EMPTY_SQUARE) {
            return board_at_coord(board, to)->player != moving_player;
        }
        return false;
    }
}

bool parse_pawn_move(struct move* move, const char* str, enum player moving_player, board board) {
    if (!_parse_pawn_move(move, str, moving_player)) {
        return false;
    }

    ASSERT_PRINTF(find_starting_square(board, move, can_pawn_move_to), "Cannot find a starting square !\nMove: %s", move->algebraic_move);
    return true;
}
