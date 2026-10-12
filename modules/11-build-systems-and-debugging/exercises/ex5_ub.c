/* ex5_ub.c - undefined behavior UBSan will flag at runtime. Fix the root cause.
 *
 * Build: gcc -std=c11 -Wall -Wextra -g -fsanitize=undefined ex5_ub.c -o ex5
 * Run:   ./ex5
 *
 * Intended behavior: print how many times 10 fits before overflowing, i.e. keep
 * doubling an int until it would overflow, and report the count.
 */
#include <stdio.h>

int main(void) {
    int x = 1;
    int count = 0;
    while (x > 0) {
        x = x * 2;          /* BUG: eventually overflows a signed int (UB) */
        count++;
    }
    printf("doubled %d times\n", count);
    return 0;
}
