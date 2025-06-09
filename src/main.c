#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/args.h"
#include "../include/compress.h"
#include "../include/error.h"
#include "../include/read.h"
#include "../include/safe_bool.h"
#include "../include/source_location.h"
#include "../include/uncompress.h"

static void print_args(const struct args* args) {
    printf(
        "args = {\n"
        "\tcompress = %d\n"
        "\tuncompress = %d\n"
        "\thelp = %d\n"
        "\tinput = '%s'\n"
        "\toutput = '%s'\n"
        "}\n",
        args->compress,
        args->uncompress,
        args->help,
        (args->input == NULL) ? "NULL" : args->input,
        (args->output == NULL) ? "NULL" : args->output
    );
}

static const struct args EMPTY_ARGS = {
    .compress = false,
    .uncompress = false,
    .help = false,
    .input = NULL,
    .output = NULL
};

static void help(void) {
    puts("./pgn_compressor -c|--compress|-u|--uncompress file [-o output]");
}

static enum safe_bool parse_bool_arg(bool* flag, const char* flag_names[], size_t n_names, const char* arg) {
    for (size_t i = 0; i < n_names; i++) {
        if (strcmp(arg, flag_names[i]) == 0) {
            if (*flag) {
                fprintf(stderr, "Flag '%s' already specified !\n", flag_names[i]);
                return ERROR;
            }
            *flag = true;
            return TRUE;
        }
    }
    return FALSE;
}

static bool parse_args(struct args* args, int argc, char* argv[]) {
    bool is_reading_input = true;
    *args = EMPTY_ARGS;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--no-log") == 0) {
            log_enabled = false;
            continue;
        } else if (strcmp(argv[i], "-o") == 0) {
            if (is_reading_input) {
                fputs("Cannot have multiple -o !\n", stderr);
                return false;
            }
            is_reading_input = false;
            continue;
        }

        enum safe_bool flag_found = FALSE;
        flag_found = parse_bool_arg(&args->compress, (const char*[]){ "-c", "--compress" }, 2, argv[i]);
        flag_found = safe_bool_or(flag_found, parse_bool_arg(&args->uncompress, (const char*[]){ "-u", "--uncompress" }, 2, argv[i]));
        flag_found = safe_bool_or(flag_found, parse_bool_arg(&args->help, (const char*[]){ "-h", "--help" }, 2, argv[i]));
        if (flag_found == FALSE) {
            if (is_reading_input) {
                if (args->input != NULL) {
                    fputs("Cannot process multiple files at a time !\n", stderr);
                    return false;
                }
                args->input = argv[i];
            } else {
                if (args->output != NULL) {
                    fputs("Cannot output multiple files at a time !\n", stderr);
                    return false;
                }
                args->output = argv[i];
            }
        }
    }
    return true;
}

int main(int argc, char* argv[]) {
    atexit(loc_exit);

    struct args args;
    if (!parse_args(&args, argc, argv)) {
        return EXIT_FAILURE;
    }
    print_args(&args);
    if (args.help || argc == 1) {
        help();
        return EXIT_SUCCESS;
    } else if (args.compress && args.uncompress) {
        fputs("Cannot compress and uncompress at the same time !\n", stderr);
        return EXIT_FAILURE;
    } else if (!args.compress && !args.uncompress) {
        fputs("Must compress or uncompress !\n", stderr);
        return EXIT_FAILURE;
    } else if (args.uncompress && args.input == NULL) {
        fputs("Reading from the standard input isn't supported when uncompressing !\n", stderr);
        return EXIT_FAILURE;
    }
    return args.compress ? compress(&args) : uncompress(&args);
}
