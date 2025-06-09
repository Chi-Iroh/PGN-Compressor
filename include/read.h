#pragma once
#include <stddef.h>

unsigned char* read_compressed_file(const char* name, size_t* size);
char* read_pgn_stdin(size_t* size);
char* read_pgn_file(const char* name, size_t* size);
