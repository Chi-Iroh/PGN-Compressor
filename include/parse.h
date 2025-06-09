#pragma once
#include <stddef.h>
#include "piece.h"

/**
 * @returns if a char is a valid rank : a letter between a and h (both included, case insensitive)
*/
bool is_file(char c);

/**
 * @returns file index represented by c (a-h), will return erroneous value if c isn't a file
*/
int to_file(char c);

/**
 * @returns if a char is a valid file : a digit between 1 and 8 (both included)
*/
bool is_rank(char c);

/**
 * @returns rank represented by c (0-7), will return erroneous value if c isn't a rank
*/
int to_rank(char c);

/**
 * @returns if there's a check ('+' is check, '#' is checkmate, but no check otherwise)
*/
bool is_check(char c);

/**
 * @returns the check mode corresponding to a char (Check for '+', CheckMate for '#', NoCheck for anything else)
*/
enum check_type to_check(char c);

/**
 * @returns if a char is a valid piece (not pawn) : KQRNB
*/
bool is_piece(char c);

/**
 * @returns piece (not pawn) represented by c (KQRNB), will return Empty if invalid or pawn
*/
enum piece_type to_piece(char c);

/**
 * @brief Checks if the given piece type is on the board at given coords, and updates move.from if so.
 * @param move Currently parsed move, MUSTN'T be NULL
 * @returns true if the piece is found at the given coords, false otherwise
*/
bool test_move_start(struct move* move, board board, int file, int rank, enum piece_type piece);
