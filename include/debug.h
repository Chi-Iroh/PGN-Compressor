#pragma once

#include <stdio.h>

#include "piece.h"
#include "uncompress.h"

void print_move(const struct move* move, FILE* file);
void print_pgn_token(struct pgn_token* token, FILE* file);
void print_board(board board);
void debug_print(struct en_passant_header* en_passant_header, struct tag* tags, size_t n_tags);
void print_token(const struct pgn_token* token);
