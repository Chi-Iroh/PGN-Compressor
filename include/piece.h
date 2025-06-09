#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "bits_constants.h"

#define BOARD_SIZE 8

enum piece_type {
    KING = _0b000,
    QUEEN = _0b001,
    BISHOP = _0b010,
    KNIGHT = _0b011,
    ROOK = _0b100,
    PAWN = _0b101,
    EMPTY_SQUARE = _0b110
};

extern const char* PIECES_NAME[];

#define INVALID_COORD -1

struct coord {
    int file;
    int rank;
};

extern const struct coord INVALID_COORD_STRUCT;

bool are_coords_equal(const struct coord* first, const struct coord* second);

enum check_type {
    NO_CHECK,
    CHECK,
    CHECKMATE
};

struct pawn_move_infos {
    bool en_passant;
    bool has_en_passant_extra_ep_notation;
    bool promoted;
    enum piece_type promotion_piece;
};

extern const struct pawn_move_infos EMPTY_PAWN_MOVE_INFOS;

enum castling {
    KINGSIDE,
    QUEENSIDE,
    CASTLING_SIZE // size of an array defined as { [KINGSIDE] = ..., [QUEENSIDE] = ... }
};

struct king_move_infos {
    bool is_castling;
    enum castling castling;
};

enum player {
    WHITE,
    BLACK,
    INVALID_PLAYER,
    PLAYER_SIZE // size of an array defined as { [WHITE] = ..., [BLACK] = ... }
};

extern const char* PLAYER_NAMES[PLAYER_SIZE];

enum player opponent_player(enum player player);

struct winner {
    bool is_draw;
    enum player winner;
};

struct move {
    enum player player;
    enum piece_type piece;
    struct coord from;
    struct coord to;
    bool capture;
    enum check_type check;
    struct extra_infos {
        enum piece_type piece_type; // EMPTY_SQUARE if no infos
        union {
            struct pawn_move_infos pawn_infos;
            struct king_move_infos king_infos;
        } infos;
    } extra_infos;
    size_t string_len;

    // only for print/test purposes
    const char* algebraic_move;
};

extern const struct move INVALID_MOVE;

struct piece {
    enum piece_type type;
    enum player player;
};

bool are_pieces_equal(const struct piece* first, const struct piece* second);

extern const enum piece_type PROMOTION_PIECE[4];

struct tag {
    char* name;
    size_t name_len;
    char* value;
    size_t value_len;
};

enum token_type {
    MOVE_KING = KING,
    MOVE_QUEEN = QUEEN,
    MOVE_BISHOP = BISHOP,
    MOVE_KNIGHT = KNIGHT,
    MOVE_ROOK = ROOK,
    MOVE_PAWN = PAWN,
    CASTLING_OR_PROMOTION = _0b110,
    CASTLING = _0b1100,
    PROMOTION = _0b1101,
    COMMENT_OR_ALTERNATIVE_MOVE_OR_NAG_OR_END_OF_GAME = _0b111,
    COMMENT_OR_ALTERNATIVE_MOVE = _0b1110,
    NAG_OR_END_OF_THE_GAME = _0b1111,
    COMMENT = _0b11100,
    ALTERNATIVE_MOVE = _0b11101,
    NAG = _0b11110,
    END_OF_THE_GAME = _0b11111,
};

struct pgn_token {
    enum token_type type;
    union {
        struct move move;
        uint8_t nag;
        char* comment;
        struct winner winner;
        struct {
            struct pgn_token* tokens;
            size_t size;
            size_t max_size;
        } alternative_moves;
    } move;
};

typedef struct piece board[BOARD_SIZE][BOARD_SIZE];

struct board_state {
    enum player current_player;
    unsigned move_turn;
    board board;
};

struct board_state empty_board_state(void);
void next_turn(struct board_state* state);
// bool apply_move(struct board_state* state, const struct move* move);

// ----------------------------------------------------------------------------

struct piece* board_at_coord(board board, struct coord coord);
struct piece* board_at(board board, int file, int rank);

/**
 * @note if the opponent king can attack the square, we must check if going to the square puts it in check.
 * However, this function may be called when moving a king, to verify if the move doesn't put it in check.
 * In that case, the opponent king might attack the square.
 * If so, we only need to verify if the opponent king can move there, we don't care if moving puts the opponent king in check because if it would have moved the game would be finished.
 */
bool is_square_attacked_by(board board, struct coord coord, enum player attacker, bool if_opponent_king_can_attack_then_check_if_square_safe);

/**
 * Counts how many pawns of the given player are ready to promote (second to last rank), stores each coord in the array and returns the count.
 */
uint8_t count_pawns_ready_to_promote(board board, enum player pawn_player, struct coord coords[BOARD_SIZE]);

/**
 * Returns the nth piece of the given player and color, the lowest n being closer to A1, the highest n being closer to H8 (ID calculated with rank * 8 + file).
 * If doesn't exist, returns false.
 */
bool nth_piece(board board, enum player player, enum piece_type piece, uint8_t nth, struct coord* coord);

/**
 * Finds the king of the given player. Returns false if not found (shouldn't happen with correct data), or true it found.
 */
struct coord find_king(board board, enum player player);

// converts each 2D coord to an 1D coord (0->63), and compares these 1D indices
int qsort_compare_piece_by_index(const void* /* struct coord* */ coord1, const void* /* struct coord* */ coord2);

#define MAX_PIECES_TO_GO_TO_SAME_SQUARE 8 // At most 8 pieces of the same type could go to the same square (either 8 knights or 8 queens)

/**
 * Returns how many pieces of the given type of the given player can move to the given square.
 */
uint8_t count_how_many_pieces_of_same_type_can_move_to_square(board board, enum player player, enum piece_type piece, struct coord* to, struct coord coords[MAX_PIECES_TO_GO_TO_SAME_SQUARE]);

// ----------------------------------------------------------------------------

extern bool (*const can_move_to[])(struct coord from, struct coord to, enum player moving_player, board board);

/**
 * Returns if the player is in check, checkmate or nothing.
 * If look_for_escape is true, then the function will look if there's any move than allows the player to escape the check.
 * Then it'll return CHECK if so, and CHECKMATE if not.
 * Passing false to this parameter just looks if the player is in check, and can return only NO_CHECK or CHECK
 */
enum check_type is_player_checked(board board, enum player player, bool look_for_escape);
