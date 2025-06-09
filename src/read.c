#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/error.h"
#include "../include/read.h"

#define STDIN_BUF_SIZE ((size_t)256)

char* read_pgn_stdin(size_t* size) {
    char* buf = NULL;
    size_t buf_size = 0;
    char tmp_buf[STDIN_BUF_SIZE];

    while (true) {
        if (feof(stdin)) {
            break;
        } else if (ferror(stdin) != 0) {
            perror("Error while reading stdin");
            if (buf != NULL) {
                free(buf);
            }
            return NULL;
        }
        tmp_buf[STDIN_BUF_SIZE - 1] = '\0';
        const size_t n = fread(tmp_buf, sizeof(char), STDIN_BUF_SIZE - 1, stdin);

        if (n == 0) {
            if (buf == NULL) {
                buf = calloc(1, sizeof(char)); // empty string
            }
            break;
        } else if (buf == NULL) {
            buf = malloc(sizeof(char) * (n + 1));
            if (buf == NULL) {
                errprintf("Error while allocating %zu bytes", n + 1);
                return NULL;
            }
            buf_size = n + 1;
            buf[n] = '\0';
            memcpy(buf, &tmp_buf[0], n);
        } else {
            char* const more_space = realloc(buf, buf_size + n); // no +1 because space for \0 is already allocated
            if (more_space == NULL) {
                errprintf("Error while allocating %zu bytes", buf_size + n);
                free(buf);
                return NULL;
            }
            buf = more_space;
            strcpy(&buf[buf_size - 1], tmp_buf);
            buf_size += n;
            buf[buf_size - 1] = '\0';
        }
    }
    *size = buf_size;
    return buf;
}

static size_t file_size(FILE* file) {
    fseek(file, 0, SEEK_END);
    const size_t size = ftell(file);
    rewind(file);
    return size;
}

static char* raw_read(FILE* file, size_t* size) {
    const size_t _file_size = file_size(file);
    char* const buf = malloc(sizeof(char) * (_file_size + 1));

    if (buf == NULL) {
        errprintf("Error while allocating %zu bytes", _file_size + 1);
        return NULL;
    }
    buf[_file_size] = '\0';
    if (fread(buf, sizeof(char), _file_size, file) != _file_size) {
        perror("File incompletely read !\n");
        free(buf);
        return NULL;
    }
    *size = _file_size;
    return buf;
}

char* read_pgn_file(const char* name, size_t* size) {
    FILE* const file = fopen(name, "r");

    if (file == NULL) {
        errprintf("Cannot open file %s", name);
        return NULL;
    }
    char* const buf = raw_read(file, size);
    fclose(file);
    return buf;
}


unsigned char* read_compressed_file(const char* name, size_t* size) {
    FILE* const file = fopen(name, "rb");

    if (file == NULL) {
        errprintf("Cannot open file %s", name);
        return NULL;
    }
    unsigned char* const buf = (unsigned char*)raw_read(file, size);
    fclose(file);
    return buf;
}
