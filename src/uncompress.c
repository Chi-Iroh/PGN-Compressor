#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../include/apply_move.h"
#include "../include/array.h"
#include "../include/bits.h"
#include "../include/error.h"
#include "../include/king.h"
#include "../include/log.h"
#include "../include/read.h"
#include "../include/piece.h"
#include "../include/safe_bool.h"
#include "../include/source_location.h"
#include "../include/uncompress.h"

#define MAX_EN_PASSANT 8
#define N_EN_PASSANT_BITS 4 // 0 min to 8 en passant max, thus 9 possibilities = 4 bits

struct en_passant_header {
    uint8_t n_en_passant : N_EN_PASSANT_BITS;
    bool has_en_passant_extra_ep_notation[MAX_EN_PASSANT];
};

static bool parse_en_passant_header(struct compressed_buf* buf, struct en_passant_header* header) {
    ASSERT_PRINTF(buf != NULL, "Compressed buffer is NULL !");
    ASSERT_PRINTF(header != NULL, "En passant header is NULL !");

    uint8_t n_en_passant; // cannot take address of bit field
    if (!read_n_bits(buf, N_EN_PASSANT_BITS, &n_en_passant)) {
        return false;
    }
    header->n_en_passant = n_en_passant;

    uint8_t has_extra_ep;
    for (uint8_t i = 0; i < n_en_passant; i++) {
        if (!read_n_bits(buf, 1, &has_extra_ep)) {
            fprintf(stderr, "Error while reading en passant bit n°%" PRIu8 " !\n", i);
            return false;
        }
        header->has_en_passant_extra_ep_notation[i] = has_extra_ep == 1;
    }
    return true;
}

static bool parse_tag(struct compressed_buf* buf, struct tag* tag) {
    ASSERT_PRINTF(buf != NULL, "Compressed buffer is NULL !");
    ASSERT_PRINTF(tag != NULL, "Tag is NULL !");

    tag->name = (char*)read_bytes_until_nul_terminator(buf, &tag->name_len);
    LOG("tag name: '%s'", tag->name);
    if (tag->name == NULL) {
        return false;
    }
    tag->value = (char*)read_bytes_until_nul_terminator(buf, &tag->value_len);
    LOG("tag value: '%s'", tag->value);
    if (tag->value == NULL) {
        return false;
    }
    return true;
}

static bool parse_tags(struct compressed_buf* buf, struct tag** tags, size_t* n_tags, size_t* max_tags) {
    ASSERT_PRINTF(buf != NULL, "Compressed buffer is NULL !");
    ASSERT_PRINTF(tags != NULL, "Tags array address is NULL !");
    ASSERT_PRINTF(n_tags != NULL, "Tags counter is NULL !");

    uint8_t next_byte;
    if (is_buf_empty(buf)) { // if empty buf here, then no tags and no moves
        return true;
    } else if (!peek_n_bits(buf, 8, &next_byte)) {
        fprintf(stderr, "Cannot look at next byte to detect tags !\n");
        return false;
    }
    if (next_byte == '\0') { // no tags
        read_n_bits(buf, 8, &next_byte); // consuming the \0
        *tags = NULL;
        *n_tags = 0;
        return true;
    }

    *n_tags = 0;
    *max_tags = 10; // 7 tags are usually present in tournaments PGN (https://en.wikipedia.org/wiki/Portable_Game_Notation#Seven_Tag_Roster)
                  // 10 is a rather arbitrary value, we just add 3 more to have a little more space for eventual extra tags
    *tags = malloc(sizeof(struct tag) * *max_tags);
    if (*tags == NULL) {
        errprintf("Cannot allocate space for %zu tags !\n", *n_tags);
        return false;
    }
    memset(*tags, 0, sizeof(struct tag) * *n_tags);

    for (; ; (*n_tags)++) {
        if (is_buf_empty(buf)) { // end of tags, and no move after
            return true;
        }

        uint8_t byte;
        if (!peek_n_bits(buf, 8, &byte)) {
            goto error;
        } else if (byte == 0) {
            read_n_bits(buf, 8, &next_byte); // consuming the \0
            break;
        } else if (!parse_tag(buf, *tags + *n_tags)) {
            goto error;
        } else if (!expand_array_if_needed(tags, *n_tags, sizeof(struct tag), max_tags, 2)) {
            goto error;
        }
    }
    return true;

error:
    free(*tags);
    *tags = NULL;
    *n_tags = 0;
    *max_tags = 0;
    return false;
}

void free_tags(struct tag** tags, size_t* n_tags, size_t* max_tags) {
    ASSERT_PRINTF_RETURN(n_tags != NULL, "Tags size is NULL !");
    if (tags == NULL) {
        return;
    }

    for (size_t i = 0; i < *n_tags; i++) {
        struct tag* const tag = *tags + i;
        if (tag) {
            free(tag->name);
            free(tag->value);
        }
    }
    free(*tags);
    *tags = NULL;
    *n_tags = 0;
    *max_tags = 0;
}

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

static void print_move(const struct move* move, FILE* file) {
    LOG("%d", move->piece);
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

static void print_pgn_token(struct pgn_token* token, FILE* file) {
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
        ASSERT_PRINTF_RETURN(token->move.alternative_moves.size > 0, "Empty alternative moves sequence !");
        fputc('(', file);
        for (size_t i = 0; i < token->move.alternative_moves.size; i++) {
            print_pgn_token(token->move.alternative_moves.tokens + i, file);
        }
        fputc(')', file);
        break;

    default:
        print_move(&token->move.move, file);
        break;
    }
}

static bool parse_castling(struct compressed_buf* buf, struct board_state* state, struct pgn_token* token) {
    uint8_t castling_bit;
    if (!read_n_bits(buf, 1, &castling_bit)) {
        return false;
    }
    const enum castling castling = castling_bit ? QUEENSIDE : KINGSIDE;
    *token = (struct pgn_token) {
        .type = CASTLING,
        .move = {
            .move = (struct move) {
                .algebraic_move = castling_bit ? "O-O-O" : "O-O",
                .capture = false,
                .player = state->current_player,
                .from = king_starting_coords[state->current_player],
                .to = king_ending_coords[state->current_player][castling],
                .extra_infos = {
                    .piece_type = KING,
                    .infos = {
                        .king_infos = (struct king_move_infos) {
                            .is_castling = true,
                            .castling = castling
                        }
                    }
                }
            }
        }
    };
    return true;
}

static bool parse_nag(struct compressed_buf* buf, struct pgn_token* token) {
    uint8_t nag;
    if (!read_n_bits(buf, 8, &nag)) {
        return false;
    }
    *token = (struct pgn_token) {
        .type = NAG,
        .move = {
            .nag = nag
        }
    };
    return true;
}

static bool parse_end_of_the_game(struct compressed_buf* buf, struct pgn_token* token) {
    uint8_t end_of_the_game;
    if (!read_n_bits(buf, 2, &end_of_the_game)) {
        return false;
    }

    struct winner winner = { .is_draw = false, .winner = INVALID_PLAYER };

    switch (end_of_the_game) {
        case _0b00:
            winner.winner = WHITE;
            break;

        case _0b01:
            winner.winner = BLACK;
            break;

        case _0b10:
            winner.is_draw = true;
            break;

        default:
            FAIL("Invalid end of the game: %" PRIx8, end_of_the_game);
    }
    *token = (struct pgn_token) {
        .type = END_OF_THE_GAME,
        .move = {
            .winner = winner
        }
    };
    return true;
}

static bool parse_comment(struct compressed_buf* buf, struct pgn_token* token) {
    size_t len;
    char* comment = (char*)read_bytes_until_nul_terminator(buf, &len);

    if (comment == NULL) {
        fprintf(stderr, "Cannot read comment !\n");
        return false;
    }
    *token = (struct pgn_token) {
        .type = CASTLING,
        .move = {
            .comment = comment
        }
    };
    return true;
}

static bool parse_promotion(struct compressed_buf* buf, struct board_state* state, struct pgn_token* token) {
    struct coord pawn_coords[BOARD_SIZE];
    const uint8_t pawns_ready_to_promote = count_pawns_ready_to_promote(state->board, state->current_player, pawn_coords);
    if (pawns_ready_to_promote == 0) {
        fprintf(stderr, "No pawn can promote but still parsed promotion !\n");
        return false;
    }

    enum piece_type promotion_piece;
    uint8_t byte_buf;
    if (!read_n_bits(buf, 2, &byte_buf)) {
        fprintf(stderr, "Cannot parse promoted piece !\n");
        return false;
    }
    promotion_piece = PROMOTION_PIECE[byte_buf];

    uint8_t nth_pawn = 0;
    if (pawns_ready_to_promote > 1) {
        const uint8_t n_extra_bits = how_many_bits_to_hold_number(pawns_ready_to_promote);
        if (!read_n_bits(buf, n_extra_bits, &nth_pawn)) {
            fprintf(stderr, "Cannot parse promoted pawn ID !\n");
            return false;
        }
    }
    struct coord pawn;
    if (!nth_piece(state->board, state->current_player, PAWN, nth_pawn, &pawn)) {
        fprintf(stderr, "Cannot determine which pawn is being promoted !\n");
        return false;
    }
    *token = (struct pgn_token) {
        .type = PROMOTION,
        .move = {
            .move = (struct move) {
                .capture = false, /* must be checked ! */
                .from = pawn,
                .to = pawn, /* must determine destination ! think about cases where the same pawn can capture twice or just go forward, 3 possibilities then !*/
                .player = state->current_player,
                .extra_infos = {
                    .piece_type = PAWN,
                    .infos = {
                        .pawn_infos = (struct pawn_move_infos) {
                            .en_passant = false,
                            .has_en_passant_extra_ep_notation = false,
                            .promoted = true,
                            .promotion_piece = promotion_piece
                        }
                    }
                }
            }
        }
    };
    return true;
}

static enum safe_bool parse_move(struct compressed_buf* buf, struct board_state* state, struct pgn_token* token);

static bool parse_alternative_moves_impl(struct compressed_buf* buf, struct board_state* state, struct pgn_token* token, unsigned nesting_level) {
    ASSERT_PRINTF(buf != NULL, "Compressed buffer is NULL !");
    ASSERT_PRINTF(state != NULL, "Board state is NULL !");
    ASSERT_PRINTF(token != NULL, "PGN token is NULL !");

    uint8_t extra_bit;
    if (!read_n_bits(buf, 1, &extra_bit)) {
        return false;
    }
    if (extra_bit == 0 && nesting_level == 0) {
        fprintf(stderr, "Attempt to end an alternative moves section, but none was opened !\n");
        return false;
    } else if (extra_bit == 0) {
        nesting_level--;
        return true;
    }
    nesting_level++;
    while (true) {
        if (parse_move(buf, state, token) != TRUE) {
            nesting_level--;
            return false;
        }
    }
    return true;
}

static bool parse_alternative_moves(struct compressed_buf* buf, struct board_state* state, struct pgn_token* token) {
    return parse_alternative_moves_impl(buf, state, token, 0);
}

static bool parse_move_impl(struct compressed_buf* buf, struct board_state* state, struct pgn_token* token) {
    uint8_t file;
    uint8_t rank;
    ASSERT_PRINTF(read_n_bits(buf, 3, &file), "Error while parsing move file !");
    ASSERT_PRINTF(read_n_bits(buf, 3, &rank), "Error while parsing move rank !");
    token->move.move.to.file = file;
    token->move.move.to.rank = rank;
    token->move.move.capture = board_at_coord(state->board, token->move.move.to)->type != EMPTY_SQUARE;
    LOG("%d", token->move.move.piece);

    struct coord coords[8];
    const uint8_t count = count_how_many_pieces_of_same_type_can_move_to_square(state->board, state->current_player, token->move.move.piece, &token->move.move.to, coords);
    LOG("%hhu piece%s can move to the square %c%hhu\n", count, count >= 2 ? "s" : "", 'a' + token->move.move.to.file, 1 + token->move.move.to.rank);
    struct coord* coord = &coords[0];

    if (count > 1) {
        uint8_t nth;
        ASSERT_PRINTF(read_n_bits(buf, how_many_bits_to_hold_number(count), &nth), "Cannot read disambiguation bits !");
        qsort(coords, count, sizeof(struct coord), qsort_compare_piece_by_index);
        coord = coords + nth;
    }
    token->move.move.from = *coord;
    LOG("The moves comes from %c%hhu", 'a' + coord->file, 1 + coord->rank);
    token->move.move.piece = board_at_coord(state->board, *coord)->type;

    // we must apply the move before calling is_player_checked, but we do it on a temp board, as the move is applied in the main uncompressing loop
    board copy;
    memcpy(copy, state->board, sizeof(board));
    apply_move(token, copy);
    token->move.move.check = is_player_checked(copy, opponent_player(state->current_player), true);
    return true;
}

static enum safe_bool parse_move(struct compressed_buf* buf, struct board_state* state, struct pgn_token* token) {
    ASSERT_PRINTF(buf != NULL, "Compressed buffer is NULL !");
    ASSERT_PRINTF(token != NULL, "PGN token is NULL !");

    memset(token, 0, sizeof(struct pgn_token));

    uint8_t token_3bits;
    if (!read_n_bits(buf, 3, &token_3bits)) {
        return false;
    }

    if (token_3bits != _0b110 && token_3bits != _0b111) {
        token->type = token_3bits;
        token->move.move.piece = token_3bits;
        return parse_move_impl(buf, state, token);
    }

    uint8_t extra_1st_bit;
    ASSERT_PRINTF(read_n_bits(buf, 1, &extra_1st_bit), "Cannot read extra 1st bit !");

    if (token_3bits == _0b110) {
        switch (extra_1st_bit) {
            case 0:
                return parse_castling(buf, state, token);

            case 1:
                return parse_promotion(buf, state, token);
        }
        FAIL("Invalid bit, got %" PRIu8 " instead of 0 or 1 !", extra_1st_bit);
    } else if (token_3bits == _0b111) {
        uint8_t extra_2nd_bit;
        ASSERT_PRINTF(read_n_bits(buf, 1, &extra_2nd_bit), "Cannot read 2nd extra bit !");
        ASSERT_PRINTF(extra_1st_bit <= 1, "Invalid extra 1st bit, got %" PRIu8 " instead of 0 or 1 !", extra_1st_bit);
        ASSERT_PRINTF(extra_2nd_bit <= 1, "Invalid extra 2nd bit, got %" PRIu8 " instead of 0 or 1 !", extra_2nd_bit);

        switch (extra_1st_bit) {
            case 0:
                switch (extra_2nd_bit) {
                    case 0:
                        return parse_comment(buf, token);
                    case 1:
                        return parse_alternative_moves(buf, state, token);
                }

            case 1:
                switch (extra_2nd_bit) {
                    case 0:
                        return parse_nag(buf, token);
                    case 1:
                        return parse_end_of_the_game(buf, token);
                }

            default:
                FAIL("Invalid 1st bit, got %" PRIu8 " instead of 0 or 1 !", extra_1st_bit);
        }
    }
    FAIL("Invalid bits !");
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

void free_token(struct pgn_token* token) {
    if (token == NULL) {
        return;
    }
    if (token->type == CASTLING) {
        free(token->move.comment);
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
        printf("Alternative move%s :\n", token->move.alternative_moves.size >= 2 ? "s" : "");
        for (size_t i = 0; i < token->move.alternative_moves.size; i++) {
            print_token(token->move.alternative_moves.tokens + i);
        }
        puts("--");
        break;

    case END_OF_THE_GAME:
        puts("End of the game");
        break;
    }
}

static bool parse_version(struct compressed_buf* buf, uint8_t* version) {
    return read_n_bits(buf, 8, version);
}

int uncompress(const struct args* args) {
    ASSERT_PRINTF_EXIT_FAILURE(args != NULL, "args is NULL !");

    size_t size;
    unsigned char* const raw_buf = read_compressed_file(args->input, &size);
    if (raw_buf == NULL) {
        fprintf(stderr, "Error while reading %s\n", args->input);
        return 1;
    }
    printf("Content of %s (%zu byte%s):\n", args->input, size, size >= 2 ? "s" : "");
    binary_print(raw_buf, size);

    struct compressed_buf buf = {
        .buf = raw_buf,
        .n_bytes = size,
        .nth_bit = 0,
        .nth_byte = 0,
        .remaining_bits = size * 8
    };
    uint8_t version = 0;
    struct en_passant_header en_passant_header;
    struct tag* tags = NULL;
    size_t n_tags = 0;
    size_t max_tags;

    bool status = true;
    status = status && parse_version(&buf, &version);
    printf("Protocol v%" PRIx8 "\n", version);
    LOG("After version, status %d\n", status);
    status = status && parse_tags(&buf, &tags, &n_tags, &max_tags);
    LOG("After tags, status: %d\n", status);
    status = status && parse_en_passant_header(&buf, &en_passant_header);
    LOG("After en passant, status: %d\n", status);
    debug_print(&en_passant_header, tags, n_tags);

    bool has_moves = false;
    struct board_state board_state = empty_board_state();
    struct pgn_token token;
    enum safe_bool state;
    while (!is_buf_empty(&buf)) {
        if ((state = parse_move(&buf, &board_state, &token)) != TRUE) {
            break;
        }
        print_token(&token);
        if (token.type == END_OF_THE_GAME) {
            break;
        }
        apply_move(&token, board_state.board);
        next_turn(&board_state);
        has_moves = true;
    }
    if (state == ERROR) {
        status = false;
    }

    free_token(has_moves ? &token : NULL);
    free_tags(&tags, &n_tags, &max_tags);
    free(raw_buf);
    LOG("%d\n", log_enabled);
    return status ? EXIT_SUCCESS : EXIT_FAILURE;
}
