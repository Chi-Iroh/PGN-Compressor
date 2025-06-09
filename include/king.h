#pragma once
#include "piece.h"

/**
 * @brief Tries to extract a syntactically correct king move from the string and stores it into the pointer.
 * @param str PGN buffer, fails if NULL or has not enough characters to read.
 * @param player current moving player
 * @param board position right before the move
 * @returns the move if it's valid, std::nullopt otherwise
*/
bool parse_king_move(struct move* move, const char* str, enum player moving_player, board board);

bool can_king_move_to(struct coord from, struct coord to, enum player moving_player, board board, bool check_is_is_dest_square_safe);

extern  struct coord king_starting_coords[PLAYER_SIZE];
extern struct coord king_ending_coords[PLAYER_SIZE][CASTLING_SIZE];
extern struct coord rook_starting_coords[PLAYER_SIZE][CASTLING_SIZE];
extern struct coord rook_ending_coords[PLAYER_SIZE][CASTLING_SIZE];
