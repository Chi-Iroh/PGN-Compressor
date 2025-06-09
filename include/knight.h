#pragma once
#include "piece.h"

/**
 * @brief Tries to extract a syntactically correct knight move from the string and stores it into the pointer.
 * @param str PGN buffer, fails if NULL or has not enough characters to read.
 * @param player current moving player
 * @param board position right before the move
 * @returns the move if it's valid, std::nullopt otherwise
*/
bool parse_knight_move(struct move* move, const char* str, enum player moving_player, board board);

bool can_knight_move_to(struct coord from, struct coord to, enum player moving_player, board board);
