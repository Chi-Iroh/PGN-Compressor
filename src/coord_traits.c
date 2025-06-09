#include "../include/coord_traits.h"
#include "../include/coord_constants.h"

static bool has_coord(const struct coord* coords, size_t n_coords, struct coord coord) {
    for (size_t i = 0; i < n_coords; i++) {
        if (are_coords_equal(coords + i, &coord)) {
            return true;
        }
    }
    return false;
}

bool is_coord_in_corner(struct coord coord) {
    const struct coord coords[4] = {
        MAKE_CONSTANT_COORD(A,1),
        MAKE_CONSTANT_COORD(A,8),
        MAKE_CONSTANT_COORD(H,1),
        MAKE_CONSTANT_COORD(H,8)
    };
    return has_coord(coords, 4, coord);
}

bool is_coord_on_edge_but_not_in_corner(struct coord coord) {
    return  (coord.file == 0 || coord.file == BOARD_SIZE - 1) ^ // XOR because if both file and rank are 0 or 7, then it's a corner
            (coord.rank == 0 || coord.rank == BOARD_SIZE - 1);
}

bool has_coord_8_squares_around_it(struct coord coord) {
    return is_coord_in_corner(coord) && !is_coord_on_edge_but_not_in_corner(coord);
}

bool is_coord_one_square_orthogonally_from_corner(struct coord coord) {
    const struct coord coords[8] = {
        MAKE_CONSTANT_COORD(B,1),
        MAKE_CONSTANT_COORD(A,2),
        MAKE_CONSTANT_COORD(A,7),
        MAKE_CONSTANT_COORD(B,8),
        MAKE_CONSTANT_COORD(G,8),
        MAKE_CONSTANT_COORD(H,7),
        MAKE_CONSTANT_COORD(H,2),
        MAKE_CONSTANT_COORD(G,1)
    };
    return has_coord(coords, 8, coord);
}

bool is_coord_one_square_diagonally_from_corner(struct coord coord) {
    const struct coord coords[4] = {
        MAKE_CONSTANT_COORD(B,2),
        MAKE_CONSTANT_COORD(B,7),
        MAKE_CONSTANT_COORD(G,7),
        MAKE_CONSTANT_COORD(G,2)
    };
    return has_coord(coords, 4, coord);
}

// is in square B2-B7-G7-G2, but not at B2,B7,G7 or G2
bool is_coord_one_square_away_from_an_edge_but_at_least_two_from_another_edge(struct coord coord) {
    return  (coord.file >= 1 && coord.file <= BOARD_SIZE - 2) &&
            (coord.rank >= 1 && coord.rank <= BOARD_SIZE - 2) &&
            !is_coord_one_square_diagonally_from_corner(coord);
}

bool is_coord_at_least_two_squares_away_from_any_edge(struct coord coord) {
    return  (coord.file >= 2 && coord.file <= BOARD_SIZE - 3) &&
            (coord.rank >= 2 && coord.rank <= BOARD_SIZE - 3);
}
