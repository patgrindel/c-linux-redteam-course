/* signed_unsigned.c - the same bits mean different numbers.
 *
 * Build:  gcc -std=c11 -Wall -Wextra signed_unsigned.c -o signed_unsigned
 * Run:    ./signed_unsigned
 *
 * A signed int can be negative; an unsigned int cannot - it uses that extra
 * range for larger positives. Storing a negative value into an unsigned type
 * does NOT clamp or error: it reinterprets the bit pattern, producing a large
 * positive number. This is the root of a whole family of real bugs (and
 * vulnerabilities), e.g. a negative length treated as a gigantic size.
 */
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

int main(void)
{
    int neg = -1;

    /* Assigning -1 to an unsigned reinterprets the bits: all-ones = the maximum. */
    unsigned int as_unsigned = (unsigned int)neg;
    printf("(int)-1 stored as unsigned is %u\n", as_unsigned);

    uint8_t as_byte = (uint8_t)neg;   /* -1 in 8 bits = 0xFF = 255 */
    printf("(int)-1 stored as uint8_t is %" PRIu8 "\n", as_byte);

    /* The classic trap: a "length" that went negative becomes enormous. */
    int requested = -4;
    size_t length = (size_t)requested;
    printf("a requested length of %d becomes size_t %zu\n", requested, length);
    return 0;
}
