#include <stddef.h>
#include <string.h>

#include "../include/common.h"
#include "../include/error.h"
#include "../include/parse.h"

struct piece_to_char {
    enum piece_type piece_type;
    char piece_char;
};

static char PIECE_TO_CHAR[] = {
    [BISHOP] = 'B',
    [KNIGHT] = 'N',
    [KING] = 'K',
    [ROOK] = 'R',
    [QUEEN] = 'Q'
};

bool parse_move(struct move* move, enum piece_type piece, const char* str, enum player moving_player) {
    size_t i = 0;
    ASSERT_PRINTF(is_piece(str[i]), "Move was expected to start with a piece (not pawn), but got '%c' !", str[i]);

    struct move parsed_move = INVALID_MOVE;
    parsed_move.player = moving_player;
    ASSERT_PRINTF(str[i] == PIECE_TO_CHAR[piece], "'%c' piece was expected to begin the move, but got '%c' !", PIECE_TO_CHAR[piece], str[i]);
    parsed_move.piece = to_piece(str[i++]);

    int file = INVALID_COORD;
    int rank = INVALID_COORD;
    if (is_file(str[i])) {
        file = to_file(str[i++]);
    }
    if (is_rank(str[i])) {
        rank = to_rank(str[i++]);
    }
    if (str[i] == 'x') {
        parsed_move.capture = true;
        i++;
    }
    if (is_check(str[i])) { // end of the move
        parsed_move.to = (struct coord) {
            .file = file,
            .rank = rank
        };
        parsed_move.check = to_check(str[i++]);
        parsed_move.string_len = i;
        goto parsing_ok;
    }
    if (is_file(str[i]) && is_rank(str[i + 1])) {
        parsed_move.from = (struct coord) {
            .file = file,
            .rank = rank
        };
        parsed_move.to = (struct coord) {
            .file = to_file(str[i]),
            .rank = to_rank(str[i + 1])
        };
        i += 2;
    } else {
        ASSERT_PRINTF(i == strlen(str) || is_check(str[i]), "Unexpected characters in move, got '%s' !", str + i);
        parsed_move.to = (struct coord) {
            .file = file,
            .rank = rank
        };
    }
    parsed_move.check = to_check(str[i]);
    i += parsed_move.check != NO_CHECK;
    parsed_move.string_len = i;
parsing_ok:
    *move = parsed_move;
    return true;
}

bool find_starting_square(board _board, struct move* move, bool (*can_move_to)(struct coord, struct coord, enum player, board)) {
    const bool is_file_valid = move->from.file != INVALID_COORD;
    const bool is_rank_valid = move->from.rank != INVALID_COORD;

    if (is_file_valid && is_rank_valid) {
        return can_move_to(move->from, move->to, move->player, _board);
    } else if (is_file_valid || is_rank_valid) {
        int* coord_not_set = is_file_valid ? &move->from.rank : &move->from.file;
        for (size_t coord = 0; coord < BOARD_SIZE; coord++) {
            *coord_not_set = coord;
            if (board_at_coord(_board, move->from)->type != EMPTY_SQUARE) {
                continue;
            }
            if (can_move_to(move->from, move->to, move->player, _board)) {
                return true;
            }
        }
        *coord_not_set = INVALID_COORD;
        return false;
    }

    // no starting coord was specified, testing the whole board
    for (size_t rank = 0; rank < BOARD_SIZE; rank++) {
        move->from.rank = rank;
        for (size_t file = 0; file < BOARD_SIZE; file++) {
            move->from.file = file;
            if (board_at_coord(_board, move->from)->type != EMPTY_SQUARE) {
                continue;
            }
            if (can_move_to(move->from, move->to, move->player, _board)) {
                return true;
            }
        }
    }
    return false;
}
