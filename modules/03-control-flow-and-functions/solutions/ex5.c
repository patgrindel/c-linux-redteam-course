/* ex5.c - recursive integer power.
 *
 * Approach: base^exp = base * base^(exp-1), with base^0 = 1 as the base case
 * that stops the recursion. Each call multiplies by one more 'base' and recurses
 * on a smaller exponent, building the result as the stack unwinds. Result is a
 * long so modest powers don't overflow an int immediately.
 */
#include <stdio.h>

static long ipow(int base, int exp)
{
    if (exp <= 0) {
        return 1;                       /* base case */
    }
    return (long)base * ipow(base, exp - 1);
}

int main(void)
{
    printf("2^10 = %ld\n", ipow(2, 10));
    printf("5^3  = %ld\n", ipow(5, 3));
    printf("7^0  = %ld\n", ipow(7, 0));
    return 0;
}
