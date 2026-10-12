/* ex4.c - Python loop translated to C.
 *
 * Approach: Python's range(1, 11) is 1..10 inclusive, so the C loop runs i from
 * 1 while i <= 10. Both total and i are declared with explicit int type - the
 * one thing C makes you say that Python infers.
 */
#include <stdio.h>

int main(void)
{
    int total = 0;
    for (int i = 1; i <= 10; i++) {
        total += i;
    }
    printf("sum of 1..10 is %d\n", total);
    return 0;
}
