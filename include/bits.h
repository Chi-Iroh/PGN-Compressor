#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../include/safe_bool.h"

struct compressed_buf {
    const uint8_t* buf;
    size_t n_bytes;
    size_t remaining_bits;
    size_t nth_byte;
    uint8_t nth_bit;
};

bool is_buf_empty(const struct compressed_buf* buf);

/**
 * Same as read_n_bits except the buffer is untouched.
 */
bool peek_n_bits(const struct compressed_buf* buf, uint8_t n_bits, uint8_t* n);

/**
 * Extracts and consumes up to 8 bits from the buffer.
 */
bool read_n_bits(struct compressed_buf* buf, uint8_t n_bits, uint8_t* n);

uint8_t* read_n_bytes(struct compressed_buf* buf, size_t n_bytes);

/**
 * Also consumes and includes the NUL terminator, it's safe to cast to a char*.
 */
uint8_t* read_bytes_until_nul_terminator(struct compressed_buf* buf, size_t* size);

/**
 * Returns ERROR if fails, FALSE if byte not found and TRUE if byte found.
 */
enum safe_bool memchr_bits(struct compressed_buf* buf, uint8_t byte, size_t* size);

bool make_compressed_buf(struct compressed_buf* dest, const uint8_t* buf, size_t buf_size);

/**
 * Counts how many bits are required to hold a value (i.e. 3 bits are necessary to hold 7).
 */
uint8_t how_many_bits_to_hold_number(uint8_t n);

/**
 * Prints binary buffer like xxd/hexdump.
 */
void binary_print(const uint8_t* buf, size_t size);
