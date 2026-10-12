/* ex5.c - fixed version of ex5_ub.c.
 *
 * UBSan reported "signed integer overflow: ... * 2 cannot be represented in type
 * 'int'": the loop multiplied past INT_MAX, which is undefined behavior (the
 * x > 0 test was never reliable because overflow is UB, not a defined wrap).
 * Fix: test BEFORE doubling whether the next double would overflow, using
 * INT_MAX / 2 as the safe threshold. No overflow ever happens.
 *
 * Build: gcc -std=c11 -Wall -Wextra -g -fsanitize=undefined,address ex5.c -o ex5
 * Run:   ./ex5
 */
#include <limits.h>
#include <stdio.h>

int main(void) {
    int x = 1;
    int count = 0;
    while (x <= INT_MAX / 2) {   /* safe: doubling stays representable */
        x = x * 2;
        count++;
    }
    printf("doubled %d times\n", count);
    return 0;
}
