#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>

#include "../include/debug.h"
#include "../include/error.h"
#include "../include/log.h"
#include "../include/strings.h"

static void _print_move(const struct move* move, FILE* file) {
    char buf[32] = { 0 };
    char* head = buf;

    if (move->piece != PAWN) {
        *head++ = PIECE_CHAR[move->piece];
    }
    if (move->capture) {
        if (move->piece == PAWN) {
            *head++ = 'a' + move->from.file;
        }
        *head++ = 'x';
    }
    *head++ = FILE_NAMES[move->to.file];
    head += sprintf(head, "%hhu", move->to.rank + 1);
    if (move->piece == PAWN && move->extra_infos.infos.pawn_infos.promoted) {
        *head++ = '=';
        *head++ = PIECE_CHAR[move->extra_infos.infos.pawn_infos.promotion_piece];
    }
    head += sprintf(head, "%s", CHECK_STRING[move->check]);
    if (move->piece == PAWN && move->extra_infos.infos.pawn_infos.en_passant && move->extra_infos.infos.pawn_infos.has_en_passant_extra_ep_notation) {
        head += sprintf(head, " e.p.");
    }

    if (file == NULL) {
        LOG("%s", buf);
    } else {
        fputs(buf, file);
    }
}

void print_move(const struct move* move, FILE* file, bool newline) {
    _print_move(move, file);
    if (newline) {
        fputc('\n', file);
    }
}

void log_move(const struct move* move) {
    _print_move(move, NULL);
}

void print_pgn_token(struct pgn_token* token, FILE* file) {
    ASSERT_PRINTF_RETURN(token != NULL, "PGN token to print is NULL !");
    ASSERT_PRINTF_RETURN(file != NULL, "Output file is NULL !");

    switch (token->type) {
    case COMMENT:
        fprintf(file, "{%s}", token->move.comment);
        break;

    case NAG:
        fprintf(file, "$%" PRIu8, token->move.nag);
        break;

    case CASTLING_OR_PROMOTION:
    case COMMENT_OR_ALTERNATIVE_MOVE_OR_NAG_OR_END_OF_GAME:
    case COMMENT_OR_ALTERNATIVE_MOVE:
    case NAG_OR_END_OF_THE_GAME:
    case ALTERNATIVE_MOVE:
        fprintf(stderr, "Cannot print ambiguous token ! Got '%s' !", token_string[token->type]);
        break;

    case ALTERNATIVE_MOVES_START:
        fputc('(', file);
        break;

    case ALTERNATIVE_MOVES_END:
        fputc(')', file);
        break;

    case CASTLING:
        fputs(token->move.move.extra_infos.infos.king_infos.castling == KINGSIDE ? "O-O" : "O-O-O", file);
        break;

    case END_OF_THE_GAME:
        if (token->move.winner.is_draw) {
            fputs("1/2-1/2", file);
        } else {
            fputs(token->move.winner.winner == WHITE ? "1-0" : "0-1", file);
        }
        break;

    default:
        print_move(&token->move.move, file, false);
        break;
    }
}

void print_board(board board, FILE* file) {
    LOGFILE_NO_LOCATION(file, "WHITE = UPPERCASE, black = lowercase");
    LOGFILE_NO_LOCATION(file, "   A B C D E F G H");
    LOGFILE_NO_LOCATION(file, "  +-+-+-+-+-+-+-+-+");

    for (uint8_t rank = 0; rank < BOARD_SIZE; rank++) {
        char line_buf[64] = { 0 };
        char* line_head = line_buf;

        line_head += sprintf(line_head, "%" PRIu8 " |", rank + 1);
        for (uint8_t _file = 0; _file < BOARD_SIZE; _file++) {
            const struct piece* piece = board_at(board, _file, rank);
            char piece_char;
            if (piece->type == PAWN) {
                piece_char = 'P';
            } else if (piece->type == EMPTY_SQUARE) {
                piece_char = ' ';
            } else {
                piece_char = PIECE_CHAR[piece->type];
            }
            if (piece->player == BLACK) {
                piece_char = tolower(piece_char);
            }
            line_head += sprintf(line_head, "%c|", piece_char);
        }
        LOGFILE_NO_LOCATION(file, "%s", line_buf);
        LOGFILE_NO_LOCATION(file, "  +-+-+-+-+-+-+-+-+");
    }
    LOGFILE_NO_LOCATION(file, "   A B C D E F G H");
}

void read_board(board dest, char board_str[BOARD_SIZE][BOARD_SIZE + 1]) {
    for (unsigned char rank = 0; rank < BOARD_SIZE; rank++) {
        for (unsigned char file = 0; file < BOARD_SIZE; file++) {
            char c = board_str[rank][file];
            struct piece piece = {
                .player = c == ' ' ? INVALID_PLAYER : (isupper(c) ? WHITE : BLACK),
                .type = EMPTY_SQUARE
            };

            c = toupper(c);
            if (c == 'P') {
                piece.type = PAWN;
            } else if (c == 'K') {
                piece.type = KING;
            } else if (c == 'Q') {
                piece.type = QUEEN;
            } else if (c == 'B') {
                piece.type = BISHOP;
            } else if (c == 'R') {
                piece.type = ROOK;
            } else if (c == 'N') {
                piece.type = KNIGHT;
            }
            dest[rank][file] = piece;
        }
    }
}

void log_en_passant_header(struct en_passant* en_passant_header) {
    ASSERT_PRINTF_RETURN(en_passant_header != NULL, "En passant header is NULL !");

    LOG("En passant header :");
    LOG_NO_LOCATION("- %u en passant", en_passant_header->n_en_passant);
    for (uint8_t i = 0; i < en_passant_header->n_en_passant; i++) {
        LOG_NO_LOCATION("- En passant n°%" PRIu8 " %s extra e.p. notation", i, en_passant_header->has_en_passant_extra_ep_notation[i] ? "has" : "has not");
    }
}

void log_tags(struct tag* tags, size_t n_tags) {
    LOG("%zu tag%s%s", n_tags, n_tags > 1 ? "s" : "", n_tags > 0 ? " :" : "");
    for (size_t i = 0; tags != NULL && i < n_tags; i++) {
        LOG_NO_LOCATION("- '%s' : '%s'", tags[i].name, tags[i].value);
    }
}
