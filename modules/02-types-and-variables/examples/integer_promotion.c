/* integer_promotion.c - small integer types are promoted to int before arithmetic.
 *
 * Build:  gcc -std=c11 -Wall -Wextra integer_promotion.c -o integer_promotion
 * Run:    ./integer_promotion
 *
 * Before C does arithmetic, operands smaller than int (char, short, _Bool) are
 * converted to int - this is "integer promotion". So "a + b" below is computed
 * as int arithmetic even though a and b are char. That is why sizeof(a + b) is
 * the size of an int, not of a char, and why the sum does not overflow at 127.
 */
#include <stdio.h>

int main(void)
{
    char a = 100;
    char b = 100;

    int sum = a + b;   /* promoted to int: 200, which a char could not hold */

    printf("a = %d, b = %d\n", a, b);
    printf("a + b = %d\n", sum);
    printf("sizeof(a)     = %zu\n", sizeof a);        /* 1 */
    printf("sizeof(a + b) = %zu\n", sizeof(a + b));   /* 4: the expression is an int */
    return 0;
}
