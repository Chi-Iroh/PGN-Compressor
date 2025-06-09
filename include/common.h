#pragma once
#include "piece.h"

/**
 * @brief Tries to extract a syntactically correct move from the string and stores it into the pointer.
 * @param piece The moved piece.
 * @param str PGN buffer, fails if NULL or has not enough characters to read.
 * @param move where to store the actual move if syntactically correct
 * @returns 0 if fails, otherwise how many characters takes the move
*/
bool parse_move(struct move* move, enum piece_type piece, const char* str, enum player moving_player);

/**
 * @brief Finds the starting square of a bishop move.
 * @returns true if it exists and is valid, false otherwise
 * @param board current position (before the move)
 * @param move current move
 * @param can_move_to callback to determine if the involved piece can move from one coord to another
 * @note if a valid starting square is found, move.from is updated
 * @note if a starting square was specified (in a move like Ba2b3), the starting coordinates are verified, true is returned if they're valid, false otherwise
 */
bool find_starting_square(board /* board */, struct move* move, bool (*can_move_to)(struct coord, struct coord, enum player, board));
//                               ^^^^^^^^^^ the first argument cannot be named 'board' here, because the board type          ^^^^^ in can_move_to functor will fail to compile
