#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <string.h>
#include "../include/parse.h"

bool is_file(char c) {
    return 'a' <= tolower(c) && tolower(c) <= 'h';
}

int to_file(char c) {
    assert(is_file(c) && "Character is not a file !");
    return c - 'a';
}

bool is_rank(char c) {
    return '1' <= c && c <= '8';
}

int to_rank(char c) {
    assert(is_rank(c) && "Character is not a rank !");
    return c - '1';
}

bool is_check(char c) {
    return c == '+' || c == '#';
}

enum check_type to_check(char c) {
    switch (c) {
        case '+': return CHECK;
        case '#': return CHECKMATE;
        default: return NO_CHECK;
    }
}

bool is_piece(char c) {
    return strchr("KQRNB", c) != NULL;
}

enum piece_type to_piece(char c) {
    switch (c) {
        case 'K': return KING;
        case 'Q': return QUEEN;
        case 'R': return ROOK;
        case 'N': return KNIGHT;
        case 'B': return BISHOP;
        default: return EMPTY_SQUARE;
    }
}

bool test_move_start(struct move* move, board board, int file, int rank, enum piece_type piece) {
    if (board[rank][file].type == piece) {
        move->from = (struct coord) { .file = file, .rank = rank };
        return true;
    }
    return false;
}
