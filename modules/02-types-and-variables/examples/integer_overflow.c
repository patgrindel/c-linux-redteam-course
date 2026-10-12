/* integer_overflow.c - what happens when a value runs off the end of its type.
 *
 * Build:  gcc -std=c11 -Wall -Wextra integer_overflow.c -o integer_overflow
 * Run:    ./integer_overflow
 *
 * UNSIGNED overflow is well-defined: it wraps around modulo 2^width, like an
 * odometer rolling over. SIGNED overflow is UNDEFINED BEHAVIOR - the compiler
 * is allowed to assume it never happens, so never rely on it. This program only
 * demonstrates the defined, unsigned case.
 */
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <limits.h>

int main(void)
{
    unsigned int max = UINT_MAX;
    printf("UINT_MAX     = %u\n", max);
    printf("UINT_MAX + 1 = %u   (wrapped to zero)\n", max + 1u);

    uint8_t counter = 255;      /* an 8-bit counter at its top value */
    printf("\nuint8_t counter = %" PRIu8 "\n", counter);
    counter++;                  /* 255 + 1 wraps to 0 */
    printf("after ++        = %" PRIu8 "   (wrapped)\n", counter);

    uint8_t a = 200, b = 100;
    uint8_t sum = (uint8_t)(a + b); /* 300 doesn't fit in 8 bits: 300 - 256 = 44 */
    printf("\n(uint8_t)(200 + 100) = %" PRIu8 "   (300 wrapped past 255)\n", sum);
    return 0;
}
