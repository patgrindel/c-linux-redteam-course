/* ex2.c - prime checker.
 *
 * Approach: is_prime returns false for n < 2, then trial-divides by every d from
 * 2 up to n/2 (d*d <= n would be faster, but n/2 is clear and fine for small n).
 * If any divisor divides n evenly, n is composite. The function takes n by value
 * and hands back a bool; main just loops and prints.
 */
#include <stdio.h>
#include <stdbool.h>

static bool is_prime(int n)
{
    if (n < 2) {
        return false;
    }
    for (int d = 2; d * d <= n; d++) {
        if (n % d == 0) {
            return false;
        }
    }
    return true;
}

int main(void)
{
    for (int n = 2; n <= 29; n++) {
        if (is_prime(n)) {
            printf("%d ", n);
        }
    }
    printf("\n");
    return 0;
}
