#include <ctype.h>
#include <inttypes.h>
#include <stddef.h>

#include "../include/bits.h"
#include "../include/log.h"
#include "../include/error.h"
#include "../include/source_location.h"

bool is_buf_empty(const struct compressed_buf* buf) {
    return buf->remaining_bits == 0;
}

bool peek_n_bits(const struct compressed_buf* buf, uint8_t n_bits, uint8_t* n) {
    ASSERT_PRINTF(buf != NULL, "Compressed buffer must not be NULL !");
    ASSERT_PRINTF(n != NULL, "Bits destination must not be NULL !");
    ASSERT_PRINTF(n_bits <= 8, "Only 8 bits or less can be extracted at once, but %" PRIu8 " were requested !", n_bits);
    ASSERT_PRINTF(buf->remaining_bits >= n_bits, "Not enough bits to read !\nBuffer only has %zu but tried to read %" PRIu8 " !", buf->remaining_bits, n_bits);

    uint16_t next = buf->buf[buf->nth_byte] << 8;
    if (n_bits >= 8 - buf->nth_bit) {
        next |= buf->buf[buf->nth_byte + 1];
    }

    next <<= buf->nth_bit; // discarding the leading bits
    next >>= buf->nth_bit; // discarding the leading bits
    next >>= 16 - n_bits - buf->nth_bit; // discarding the trailing bits
    *n = (uint8_t)next;
    return true;
}

bool read_n_bits(struct compressed_buf* buf, uint8_t n_bits, uint8_t* n) {
    if (!peek_n_bits(buf, n_bits, n)) {
        return false;
    }

    // char bits[9] = { 0 };
    // for (uint8_t nth = 7; ; nth--) {
        // bits[7 - nth] = "01"[(*n >> nth) & 1];
        // if (nth == 0) {
            // break;
        // }
    // }
    // LOG("Read %" PRIu8 " bits: %s at %zuth byte, %hhuth bit", n_bits, bits + 8 - n_bits, buf->nth_byte, buf->nth_bit);

    buf->nth_bit += n_bits;
    if (buf->nth_bit >= 8) {
        buf->nth_bit -= 8;
        buf->nth_byte++;
    }
    buf->remaining_bits -= n_bits;
    return true;
}

enum safe_bool memchr_bits(struct compressed_buf* buf, uint8_t byte, size_t* size) {
    ASSERT_PRINTF(buf != NULL, "Compressed buffer is NULL !");
    ASSERT_PRINTF(size != NULL, "size destination is NULL !");

    enum safe_bool status = FALSE;
    const size_t copy_nth_byte = buf->nth_byte;
    const size_t remaining_bits = buf->remaining_bits;
    *size = 0;

    uint8_t cur_byte;
    while (buf->remaining_bits >= 8) {
        if (!peek_n_bits(buf, 8, &cur_byte)) {
            status = ERROR;
            *size = 0;
            goto end;
        } else if (cur_byte == byte) {
            status = TRUE;
            goto end;
        }
        buf->nth_byte++;
        buf->remaining_bits -= 8;
        (*size)++;
    }
    *size = 0;

end:
    buf->nth_byte = copy_nth_byte;
    buf->remaining_bits = remaining_bits;
    return status;
}

uint8_t* read_n_bytes(struct compressed_buf* buf, size_t n_bytes) {
    ASSERT_PRINTF_NULL(buf != NULL, "Compressed buffer is NULL !");
    ASSERT_PRINTF_NULL(n_bytes * 8 <= buf->remaining_bits, "Cannot read %zu bytes, there are only %zu bits (%f bytes) !", n_bytes, buf->remaining_bits, buf->remaining_bits / 8.);

    uint8_t* bytes = malloc(n_bytes);
    if (bytes == NULL) {
        errprintf("Cannot allocate %zu bytes !", n_bytes);
        return NULL;
    }

    for (size_t i = 0; i < n_bytes; i++) {
        uint8_t byte;
        if (!read_n_bits(buf, 8, &byte)) {
            free(bytes);
            return NULL;
        }
        bytes[i] = byte;
    }
    return bytes;
}

uint8_t* read_bytes_until_nul_terminator(struct compressed_buf* buf, size_t* size) {
    ASSERT_PRINTF_NULL(buf != NULL, "Compressed buffer is NULL !");

    size_t len;
    const enum safe_bool ret = memchr_bits(buf, 0, &len);
    if (ret != TRUE) {
        fprintf(stderr, "Error while reading tag name (returned %d) !\n", ret);
        return NULL;
    }
    *size = len;
    return read_n_bytes(buf, len + 1); // +1 to include the NUL terminator
}

bool make_compressed_buf(struct compressed_buf* dest, const uint8_t* buf, size_t buf_size) {
    ASSERT_PRINTF(dest != NULL, "Destination buffer must not be NULL !");
    ASSERT_PRINTF(buf != NULL, "Buffer must not be NULL !");

    *dest = (struct compressed_buf) {
        .buf = buf,
        .n_bytes = buf_size,
        .nth_bit = 0,
        .nth_byte = 0,
        .remaining_bits = 8 * buf_size
    };
    return true;
}

uint8_t how_many_bits_to_hold_number(uint8_t n) {
    uint8_t count = 0;

    while (n) {
        count += n & 1;
        n >>= 1;
    }
    return count;
}

void binary_print(const uint8_t* buf, size_t size) {
    size_t i = 0;
    const bool is_size_multiple_of_8 = size % 8 == 0;
    const size_t n_rows = size / 8 + !is_size_multiple_of_8;
    for (size_t row = 0; row < n_rows; row++) {
        const size_t n_cols = (!is_size_multiple_of_8 && row == n_rows - 1) ? size % 8 : 8; // if not multiple of 8 and in last row, then the remainder, otherwise 8
        for (size_t col = 0; col < 8; col++) {
            if (col > 0) {
                putchar(' ');
            }
            if (col >= n_cols) {
                printf("  ");
            } else {
                printf("%02X", buf[i++]);
            }
        }

        printf(" | ");
        i -= n_cols;
        for (size_t col = 0; col < n_cols; col++) {
            const char c = buf[i++];
            putchar(isprint(c) ? c : '?');
        }
        putchar('\n');
    }
}
