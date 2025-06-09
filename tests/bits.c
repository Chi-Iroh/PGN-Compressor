#include <criterion/criterion.h>
#include "../include/bits.h"

#define _0b0011_1010 0x3A
#define _0b0011_1100 0x3C
#define _0b00 0
#define _0b11 3
#define _0b10_1000 0x28
#define N_BYTES 7

Test(bits, read) {
    const uint8_t raw_buf[N_BYTES] = {
        _0b0011_1010, _0b0011_1100, 'A', 'B', 'C', 'D', 0
    };
    struct compressed_buf buf;
    cr_assert(make_compressed_buf(&buf, raw_buf, N_BYTES));

    uint8_t n;
    cr_assert(read_n_bits(&buf, 2, &n));
    cr_assert_eq(n, _0b00, "Got 0x%0.2hhX (%hhu) instead of 0x%0.2hhX !", n, n, _0b00);

    cr_assert(read_n_bits(&buf, 2, &n));
    cr_assert_eq(n, _0b11, "Got 0x%0.2hhX (%hhu) instead of 0x%0.2hhX !", n, n, _0b11);

    cr_assert(read_n_bits(&buf, 6, &n));
    cr_assert_eq(n, _0b10_1000, "Got 0x%0.6hhX (%hhu) instead of 0x%0.6hhX !", n, n, _0b10_1000);

    cr_assert(peek_n_bits(&buf, 6, &n));
    cr_assert_eq(n, _0b0011_1100, "Got 0x%0.6hhX (%hhu) instead of 0x%0.6hhX !", n, n, _0b0011_1100);

    cr_assert(read_n_bits(&buf, 6, &n));
    cr_assert_eq(n, _0b0011_1100, "Got 0x%0.6hhX (%hhu) instead of 0x%0.6hhX !", n, n, _0b0011_1100);

    cr_assert(peek_n_bits(&buf, 8, &n));
    cr_assert_eq(n, 'A', "Got 0x%0.8hhX (%hhu) instead of 0x%0.6hhX !", n, n, 'A');

    size_t size;
    cr_assert_eq(memchr_bits(&buf, 0, &size), TRUE);
    cr_assert_eq(size, 4); // 4 bytes until 0 at this point

    cr_assert_eq(memchr_bits(&buf, 'E', &size), FALSE); // 'E' not found
    cr_assert_eq(size, 0, "Size is %zu instead of 0 !", size);
}
