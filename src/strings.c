#include "../include/strings.h"

const char* token_string[] = {
    [MOVE_KING] = "king move",
    [MOVE_QUEEN] = "queen move",
    [MOVE_BISHOP] = "bishop move",
    [MOVE_KNIGHT] = "knight move",
    [MOVE_ROOK] = "rook move",
    [MOVE_PAWN] = "pawn move",
    [CASTLING_OR_PROMOTION] = "castling / promotion",
    [CASTLING] = "castling",
    [PROMOTION] = "promotion",
    [COMMENT_OR_ALTERNATIVE_MOVE_OR_NAG_OR_END_OF_GAME] = "comment / alternative move / NAG / end of the game",
    [COMMENT_OR_ALTERNATIVE_MOVE] = "comment / alternative move",
    [NAG_OR_END_OF_THE_GAME] = "NAG / end of the game",
    [COMMENT] = "comment",
    [ALTERNATIVE_MOVE] = "alternative move",
    [NAG] = "NAG",
    [END_OF_THE_GAME] = "end of the game"
};

const char PIECE_CHAR[] = {
    [KING] = 'K',
    [KNIGHT] = 'N',
    [ROOK] = 'R',
    [BISHOP] = 'B',
    [QUEEN] = 'Q'
};

const char FILE_NAMES[BOARD_SIZE + 1] = "abcdefgh";

const char* CHECK_STRING[] = {
    [NO_CHECK] = "",
    [CHECK] = "+",
    [CHECKMATE] = "#"
};
