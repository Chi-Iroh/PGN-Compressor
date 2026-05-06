#pragma once

#include <stdio.h>

#include "en_passant.h"
#include "piece.h"
#include "uncompress.h"

void print_move(const struct move* move, FILE* file);
void print_pgn_token(struct pgn_token* token, FILE* file);
void print_board(board board);
void read_board(board dest, char board_str[BOARD_SIZE][BOARD_SIZE + 1]);
void debug_print(struct en_passant* en_passant_header, struct tag* tags, size_t n_tags);
void print_token(const struct pgn_token* token);
