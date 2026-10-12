/* ub_demo.c - writes one past the end of a stack array (off-by-one).
 *
 * Looks fine without instrumentation; AddressSanitizer stops at the exact line.
 *
 * Build: gcc -std=c11 -Wall -Wextra -g -fsanitize=address,undefined ub_demo.c -o ub_demo
 * Run:   ./ub_demo
 */
#include <stdio.h>

int main(void) {
    int a[4] = {0};
    int sum = 0;
    for (int i = 0; i <= 4; i++) {   /* BUG: i == 4 is out of bounds (fix: i < 4) */
        a[i] = i;
        sum += a[i];
    }
    printf("sum = %d\n", sum);
    return 0;
}
