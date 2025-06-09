#pragma once

#include <stdbool.h>
#include "piece.h"

// traits for ambiguous moves, see doc (§Ambiguous Moves)

bool is_coord_in_corner(struct coord coord);
bool is_coord_on_edge_but_not_in_corner(struct coord coord);
bool has_coord_8_squares_around_it(struct coord coord);

// specific traits for the knight ambiguous moves

// 1 square away horizontally or vertically (NOT diagonally)
bool is_coord_one_square_orthogonally_from_corner(struct coord coord);
bool is_coord_one_square_diagonally_from_corner(struct coord coord);

bool is_coord_one_square_away_from_an_edge_but_at_least_two_from_another_edge(struct coord coord);
bool is_coord_at_least_two_squares_away_from_any_edge(struct coord coord);
