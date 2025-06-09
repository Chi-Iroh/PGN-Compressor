#include <stdio.h>
#include <stdlib.h>

#include "../include/args.h"
#include "../include/bits.h"
#include "../include/compress.h"
#include "../include/parse.h"
#include "../include/read.h"

int compress(const struct args* args) {
    size_t size;
    char* const buf = args->input == NULL ? read_pgn_stdin(&size) : read_pgn_file(args->input, &size);
    if (buf == NULL) {
        fprintf(stderr, "Error while reading %s\n", args->input == NULL ? "stdin" : args->input);
        return 1;
    }
    printf("Content of %s (%zu byte%s):\n", args->input == NULL ? "stdin" : args->input, size, size >= 2 ? "s" : "");
    puts(buf);
    free(buf);
    return 0;
}
