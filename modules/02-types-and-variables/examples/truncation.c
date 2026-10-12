/* truncation.c - what is lost when a value moves into a smaller or different type.
 *
 * Build:  gcc -std=c11 -Wall -Wextra truncation.c -o truncation
 * Run:    ./truncation
 *
 * Putting a value into a type that cannot hold it drops information. We use
 * EXPLICIT casts here to say "yes, I mean to narrow this" - without a cast the
 * compiler may warn, and silently narrowing is how bugs sneak in.
 */
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

int main(void)
{
    int big = 1000;
    uint8_t narrow = (uint8_t)big;          /* keeps only the low 8 bits: 1000 & 0xFF */
    printf("int 1000 narrowed to uint8_t = %" PRIu8 "\n", narrow);

    /* float -> int truncates toward zero; it does NOT round. */
    double price = 3.99;
    int whole = (int)price;
    printf("(int)3.99 = %d   (fraction dropped, not rounded)\n", whole);

    double neg = -2.7;
    printf("(int)-2.7 = %d   (toward zero, so -2)\n", (int)neg);
    return 0;
}
