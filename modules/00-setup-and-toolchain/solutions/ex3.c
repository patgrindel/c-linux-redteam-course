/* ex3.c - the starter, cleaned up.
 *
 * Two fixes:
 *   1. 'total' was read before it held a value (-Wuninitialized). We initialize
 *      it to 0 so "total + 10" is well-defined.
 *   2. main declared argc/argv but never used them (-Wunused-parameter under
 *      -Wextra). This program needs no arguments, so we switch to main(void),
 *      which is the correct signature for a program that ignores its arguments.
 */
#include <stdio.h>

int main(void)
{
    int total = 0;
    total = total + 10;

    printf("total is %d\n", total);
    return 0;
}
