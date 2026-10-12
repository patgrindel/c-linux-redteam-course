/* bitwise.c - operating on individual bits. Learn this well; it recurs constantly.
 *
 * Build:  gcc -std=c11 -Wall -Wextra bitwise.c -o bitwise
 * Run:    ./bitwise
 *
 * Bitwise operators work on the binary representation directly:
 *   &  AND     1 where BOTH bits are 1        (use to test/clear bits with a mask)
 *   |  OR      1 where EITHER bit is 1         (use to set bits)
 *   ^  XOR     1 where the bits DIFFER         (use to toggle bits)
 *   ~  NOT     flips every bit
 *   << >>      shift left/right by N positions (<< n multiplies, >> n divides by 2^n)
 *
 * Do bitwise work on UNSIGNED types: shifting into or past the sign bit of a
 * signed value is trouble. A mask like (1u << 3) selects bit number 3.
 */
#include <stdio.h>
#include <stdint.h>

/* Print the 8 bits of a byte, most-significant first. */
static void print_bits_u8(uint8_t v)
{
    for (int i = 7; i >= 0; i--) {
        putchar(((v >> i) & 1u) ? '1' : '0');
    }
}

/* Permission flags, each a single distinct bit. */
#define FLAG_READ  (1u << 0)   /* 0000 0001 */
#define FLAG_WRITE (1u << 1)   /* 0000 0010 */
#define FLAG_EXEC  (1u << 2)   /* 0000 0100 */

int main(void)
{
    uint8_t x = 0x0F;   /* 0000 1111 */
    uint8_t y = 0x33;   /* 0011 0011 */

    printf("x        = "); print_bits_u8(x);       printf("  (0x%02X)\n", x);
    printf("y        = "); print_bits_u8(y);       printf("  (0x%02X)\n", y);
    printf("x & y    = "); print_bits_u8(x & y);   printf("\n");
    printf("x | y    = "); print_bits_u8(x | y);   printf("\n");
    printf("x ^ y    = "); print_bits_u8(x ^ y);   printf("\n");
    printf("~x       = "); print_bits_u8((uint8_t)~x); printf("\n");
    printf("x << 1   = "); print_bits_u8((uint8_t)(x << 1)); printf("\n");
    printf("x >> 2   = "); print_bits_u8(x >> 2);   printf("\n");

    /* Flags: set, test, toggle, clear - the core idiom. */
    uint8_t perms = 0;
    perms |= FLAG_READ | FLAG_WRITE;          /* set read and write          */
    printf("\nperms after set R,W      = "); print_bits_u8(perms); printf("\n");

    printf("has WRITE? %d\n", (perms & FLAG_WRITE) != 0);   /* test */
    printf("has EXEC?  %d\n", (perms & FLAG_EXEC)  != 0);

    perms ^= FLAG_WRITE;                       /* toggle write off */
    printf("perms after toggle WRITE = "); print_bits_u8(perms); printf("\n");

    perms &= (uint8_t)~FLAG_READ;              /* clear read */
    printf("perms after clear READ   = "); print_bits_u8(perms); printf("\n");
    return 0;
}
