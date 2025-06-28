#pragma once
#include "../include/piece.h"

void move_piece(board board, const struct coord* from, const struct coord* to);

void apply_move_on_raw_board(const struct pgn_token* token, board board);
void apply_move(const struct pgn_token* token, struct board_state* state);
