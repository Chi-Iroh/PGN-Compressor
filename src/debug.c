#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>

#include "../include/debug.h"
#include "../include/error.h"
#include "../include/log.h"
#include "../include/strings.h"

void print_move(const struct move* move, FILE* file) {
    LOG("%s", PIECES_NAME[move->piece]);
    if (move->piece == KING && move->extra_infos.infos.king_infos.is_castling) {
        fputs(move->extra_infos.infos.king_infos.castling == KINGSIDE ? "O-O\n" : "O-O-O\n", file);
        return;
    }

    if (move->piece != PAWN) {
        fputc(PIECE_CHAR[move->piece], file);
    }
    if (move->capture) {
        fputc('x', file);
    }
    fputc(FILE_NAMES[move->to.file], file);
    fprintf(file, "%hhu", move->to.rank + 1);
    if (move->piece == PAWN && move->extra_infos.infos.pawn_infos.promoted) {
        fputc('=', file);
        fputc(PIECE_CHAR[move->extra_infos.infos.pawn_infos.promotion_piece], file);
    }
    fputs(CHECK_STRING[move->check], file);
    if (move->piece == PAWN && move->extra_infos.infos.pawn_infos.en_passant && move->extra_infos.infos.pawn_infos.has_en_passant_extra_ep_notation) {
        fputs(" e.p.", file);
    }
    fputc('\n', file);
}

void print_pgn_token(struct pgn_token* token, FILE* file) {
    ASSERT_PRINTF_RETURN(token != NULL, "PGN token to print is NULL !");
    ASSERT_PRINTF_RETURN(file != NULL, "Output file is NULL !");

    switch (token->type) {
    case CASTLING:
        fprintf(file, "{%s}", token->move.comment);
        break;

    case NAG:
        fprintf(file, "$%" PRIu8, token->move.nag);
        break;

    case CASTLING_OR_PROMOTION:
    case COMMENT_OR_ALTERNATIVE_MOVE_OR_NAG_OR_END_OF_GAME:
    case COMMENT_OR_ALTERNATIVE_MOVE:
    case NAG_OR_END_OF_THE_GAME:
        fprintf(stderr, "Cannot print ambiguous token ! Got '%s' !\n", token_string[token->type]);
        break;

    case ALTERNATIVE_MOVE:
        fputc("()"[token->move.alternative_moves_is_end], file);
        break;

    default:
        print_move(&token->move.move, file);
        break;
    }
}

void print_board(board board) {
    puts("   A B C D E F G H");
    puts("  +-+-+-+-+-+-+-+-+");
    for (uint8_t rank = 0; rank < BOARD_SIZE; rank++) {
        printf("%" PRIu8 " |", rank + 1);
        for (uint8_t file = 0; file < BOARD_SIZE; file++) {
            const struct piece* piece = board_at(board, file, rank);
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
            printf("%c|", piece_char);
        }
        putchar('\n');
        puts("  +-+-+-+-+-+-+-+-+");
    }
    puts("   A B C D E F G H");
}

void debug_print(struct en_passant_header* en_passant_header, struct tag* tags, size_t n_tags) {
    ASSERT_PRINTF_RETURN(en_passant_header != NULL, "En passant header is NULL !");

    printf(
        "En passant header :\n"
        "- %u en passant\n",
        en_passant_header->n_en_passant
    );
    for (uint8_t i = 0; i < en_passant_header->n_en_passant; i++) {
        printf("- En passant n°%" PRIu8 " %s extra e.p. notation\n", i, en_passant_header->has_en_passant_extra_ep_notation[i] ? "has" : "has not");
    }

    printf("\n%zu tag%s%s\n", n_tags, n_tags > 1 ? "s" : "", n_tags > 0 ? " :" : "");
    for (size_t i = 0; tags != NULL && i < n_tags; i++) {
        printf("- '%s' : '%s'\n", tags[i].name, tags[i].value);
    }
}

void print_token(const struct pgn_token* token) {
    switch (token->type) {
    case PROMOTION:
        puts("Promotion");
        goto print_move;

    case CASTLING:
        puts("Castling");
        goto print_move;

    print_move:
    case MOVE_KING:
    case MOVE_QUEEN:
    case MOVE_BISHOP:
    case MOVE_KNIGHT:
    case MOVE_ROOK:
    case MOVE_PAWN:
        print_move(&token->move.move, stdout);
        break;

    case COMMENT:
        printf("Comment '%s'\n", token->move.comment);
        break;

    case NAG:
        printf("NAG %hhu\n", token->move.nag);
        break;

    case CASTLING_OR_PROMOTION:
        puts("Castling / promotion");
        break;

    case COMMENT_OR_ALTERNATIVE_MOVE_OR_NAG_OR_END_OF_GAME:
        puts("Comment / alternative move / NAG / end of the game");
        break;

    case COMMENT_OR_ALTERNATIVE_MOVE:
        puts("Comment / alternative move");
        break;

    case NAG_OR_END_OF_THE_GAME:
        puts("NAG / end of the game");

    case ALTERNATIVE_MOVE:
        printf("%s of alternative moves\n", token->move.alternative_moves_is_end ? "End" : "Beginning");
        break;

    case END_OF_THE_GAME:
        puts("End of the game");
        break;
    }
}
