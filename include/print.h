#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "en_passant.h"
#include "piece.h"
#include "uncompress.h"

void print_move(const struct move* move, FILE* file, bool newline);
void log_move(const struct move* move);
void print_pgn_token(struct pgn_token* token, FILE* file);

void log_en_passant_header(struct en_passant* en_passant_header);
void log_tags(struct tag* tags, size_t n_tags);
