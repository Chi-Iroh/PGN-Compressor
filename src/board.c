#include <inttypes.h>
#include <memory.h>

#include "../include/apply_move.h"
#include "../include/coord_traits.h"
#include "../include/error.h"
#include "../include/log.h"
#include "../include/piece.h"

#include "../include/bishop.h"
#include "../include/king.h"
#include "../include/knight.h"
#include "../include/pawn.h"
#include "../include/queen.h"
#include "../include/rook.h"

struct piece* board_at_coord(board board, struct coord coord) {
    return board_at(board, coord.file, coord.rank);
}

struct piece* board_at(board board, int file, int rank) {
    ASSERT_PRINTF(file >= 0 && file <= 7, "Invalid file, must be between 0 and 7, but is %d instead !", file);
    ASSERT_PRINTF(rank >= 0 && rank <= 7, "Invalid rank, must be between 0 and 7, but is %d instead !", rank);
    return &board[rank][file];
}

bool (*const can_move_to[])(struct coord from, struct coord to, enum player moving_player, board board) = {
    [BISHOP] = can_bishop_move_to,
    [KNIGHT] = can_knight_move_to,
    [PAWN] = can_pawn_move_to,
    [QUEEN] = can_queen_move_to,
    [ROOK] = can_rook_move_to
};

bool is_square_attacked_by(board board, const struct coord coord, enum player attacker, bool if_opponent_king_can_attack_then_check_if_square_safe) {
    for (size_t rank_index = 0; rank_index < BOARD_SIZE; rank_index++) {
        for (size_t file_index = 0; file_index < BOARD_SIZE; file_index++) {
            const struct coord from = { .file = file_index, .rank = rank_index };
            const struct piece square = *board_at_coord(board, from);
            if (square.player == attacker && square.type != EMPTY_SQUARE) {
                if (square.type == KING) {
                    const bool is_attacked = can_king_move_to(from, coord, attacker, board, if_opponent_king_can_attack_then_check_if_square_safe);
                    if (is_attacked) {
                        return true;
                    }
                } else {
                    const bool is_attacked = can_move_to[square.type](from, coord, attacker, board);
                    if (is_attacked) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

uint8_t count_pawns_ready_to_promote(board board, enum player pawn_player, struct coord coords[BOARD_SIZE]) {
    ASSERT_PRINTF_EXIT_PROGRAM(pawn_player == WHITE || pawn_player == BLACK, "Invalid player, got %d !\n", pawn_player);
    const uint8_t rank = pawn_player == WHITE ? BOARD_SIZE - 2 : 1;
    const struct piece pawn = {
        .type = PAWN,
        .player = pawn_player
    };
    uint8_t count = 0;


    for (uint8_t file = 0; file < BOARD_SIZE; file++) {
        if (are_pieces_equal(&board[rank][file], &pawn)) {
            coords[count] = (struct coord) {
                .file = file,
                .rank = rank
            };
            count++;
        }
    }
    return count;
}

bool nth_piece(board board, enum player player, enum piece_type piece, uint8_t nth, struct coord* coord) {
    const struct piece expected_piece = {
        .player = player,
        .type = piece
    };
    uint8_t count = 0;

    for (uint8_t rank = 0; rank < BOARD_SIZE; rank++) {
        for (uint8_t file = 0; file < BOARD_SIZE; file++) {
            struct piece* const current_piece = &board[rank][file];
            if (are_pieces_equal(current_piece, &expected_piece)) {
                if (count == nth) {
                    *coord = (struct coord) {
                        .file = file,
                        .rank = rank
                    };
                    return true;
                }
                count++;
            }
        }
    }
    return false;
}

struct coord find_king(board board, enum player player) {
    struct coord king_coord;
    ASSERT_PRINTF_EXIT_PROGRAM((nth_piece(board, player, KING, 0, &king_coord)), "Couldn't find %s's king !", PLAYER_NAMES[player]);
    return king_coord;
}

static uint8_t coord_to_1d_index(const struct coord* coord) {
    return coord->rank * BOARD_SIZE + coord->file;
}

int qsort_compare_piece_by_index(const void* /* struct coord* */ coord1, const void* /* struct coord* */ coord2) {
    const uint8_t index1 = coord_to_1d_index((const struct coord*)coord1);
    const uint8_t index2 = coord_to_1d_index((const struct coord*)coord2);

    if (index1 == index2) {
        return 0;
    }
    return index1 < index2 ? -1 : 1;
}

bool are_pieces_equal(const struct piece* first, const struct piece* second) {
    if (first == NULL) {
        return second == NULL;
    } else if (second == NULL) {
        return false;
    }
    return first->player == second->player && first->type == second->type;
}

static uint8_t how_many_pieces_at_most_could_theoretically_move_to_square(struct coord coord, enum piece_type piece) {
    const bool in_corner = is_coord_in_corner(coord);
    const bool on_edge_but_not_in_corner = is_coord_on_edge_but_not_in_corner(coord);

    switch (piece) {
    case PAWN: // always 1 pawn (no problem with captures, pawns will start from different files and/or land on different squares)
    case KING: // always 1 king
        return 1;

    case QUEEN:
        return in_corner ? 3 : (on_edge_but_not_in_corner ? 5 : 8);

    case BISHOP:
        return in_corner ? 1 : (on_edge_but_not_in_corner ? 2 : 4);

    case ROOK:
        return in_corner ? 2 : (on_edge_but_not_in_corner ? 3 : 4);

    case KNIGHT:
        if (in_corner) {
            return 2;
        } else if (is_coord_one_square_orthogonally_from_corner(coord)) {
            return 3;
        } else if (is_coord_one_square_diagonally_from_corner(coord)) {
            return 4;
        } else if (is_coord_one_square_away_from_an_edge_but_at_least_two_from_another_edge(coord)) {
            return 6;
        } else {
            return 8;
        }

    case EMPTY_SQUARE:
        FAIL("piece was an empty square !");
        return 0;
    }
}

uint8_t count_how_many_pieces_of_same_type_can_move_to_square(board board, enum player player, enum piece_type piece, struct coord* to, struct coord coords[MAX_PIECES_TO_GO_TO_SAME_SQUARE]) {
    uint8_t count = 0;
    for (int rank = 0; rank < BOARD_SIZE; rank++) {
        for (int file = 0; file < BOARD_SIZE; file++) {
            struct coord from = {
                .file = file,
                .rank = rank
            };
            struct piece* const _piece = board_at(board, file, rank);
            if (_piece->player == player && _piece->type == piece) {
                if (can_move_to[_piece->type](from, *to, player, board)) {
                    coords[count++] = from;
                    LOG("%c%hhu can go to %c%hhu\n", 'a' + from.file, 1 + from.rank, 'a' + to->file, 1 + to->rank);
                }
            }
        }
    }
    return count;
}

#define MAX_SQUARES_PER_PIECE_MOVE 27 // A piece can move to at most 27 squares, a queen in D4/D5/E4/E5 can move to at most 27 squares

static void list_all_possible_moves_for_a_piece(board board, struct coord piece_coord, struct coord possible_moves[MAX_SQUARES_PER_PIECE_MOVE], uint8_t* n_possible_moves) {
    const struct piece piece = *board_at_coord(board, piece_coord);
    *n_possible_moves = 0;

    if (piece.type == EMPTY_SQUARE) {
        return;
    }

    struct coord coord = { .file = 0, .rank = 0 };
    for (; coord.rank < BOARD_SIZE; coord.rank++) {
        for (coord.file = 0; coord.file < BOARD_SIZE; coord.file++) {
            if (are_coords_equal(&coord, &piece_coord)) {
                continue;
            } else if (piece.type == KING) {
                if (!can_king_move_to(piece_coord, coord, piece.player, board, true)) {
                    continue;
                }
            } else if (!can_move_to[piece.type](piece_coord, coord, piece.player, board)) {
                continue;
            }
            possible_moves[(*n_possible_moves)++] = coord;
        }
    }
}

static bool can_player_escape_check(board _board, enum player checked_player) {
    struct coord coord = { .file = 0, .rank = 0 };

    for (; coord.rank < BOARD_SIZE; coord.rank++) {
        for (coord.file = 0; coord.file < BOARD_SIZE; coord.file++) {
            const struct piece* const piece = board_at_coord(_board, coord);
            if (piece->type == EMPTY_SQUARE) { // empty squares are useless to escape check
                continue;
            } else if (piece->player != checked_player) { // ignoring pieces of the opponent player
                continue;
            }

            struct coord possible_moves[MAX_SQUARES_PER_PIECE_MOVE];
            uint8_t n_possible_moves;
            list_all_possible_moves_for_a_piece(_board, coord, possible_moves, &n_possible_moves);

            for (uint8_t i = 0; i < n_possible_moves; i++) {
                board copy;
                memcpy(copy, _board, sizeof(board));
                move_piece(copy, &coord, &possible_moves[i]);
                if (is_player_checked(copy, checked_player, false) == NO_CHECK) {
                    return true;
                }
            }
        }
    }
    return false;
}

#define MAX_CHECKING_PIECES 15 // king cannot check, so at most 7 pieces + 8 promoted pawns can check = at most 15 pieces

enum check_type is_player_checked(board board, enum player player, bool look_for_escape) {
    const enum player opponent = opponent_player(player);
    const struct coord king_coord = find_king(board, player);
    struct coord checking_coords[MAX_CHECKING_PIECES];
    struct piece checking_pieces[MAX_CHECKING_PIECES];
    uint8_t n_checking_pieces = 0;

    struct coord coord = { .file = 0, .rank = 0 };
    for (; coord.rank < BOARD_SIZE; coord.rank++) {
        for (coord.file = 0; coord.file < BOARD_SIZE; coord.file++) {
            const struct piece* const piece = board_at_coord(board, coord);
            if (piece->type == EMPTY_SQUARE || piece->type == KING) { // neither empty squares nor kings can put in check
                continue;
            } else if (piece->player == player) { // ignoring pieces of the same player
                continue;
            } else if (!can_move_to[piece->type](coord, king_coord, opponent, board)) {
                continue;
            }

            if (!look_for_escape) {
                return CHECK;
            }

            checking_coords[n_checking_pieces] = coord;
            checking_pieces[n_checking_pieces++] = *piece;

            if (can_player_escape_check(board, player)) {
                return CHECK;
            }
            return CHECKMATE;
        }
    }

    if (n_checking_pieces == 0) {
        return NO_CHECK;
    }
    FAIL("Error in check detection");
}
