/* ex3_combined.c - the single-file version to split apart (Exercise 3).
 *
 * Right now the declaration, definition, and use of add() all live here. Your
 * task is to separate the interface (a .h) from the implementation (a .c) from
 * the caller (ex3.c), as real C projects do.
 *
 * Builds as-is:  gcc -std=c11 -Wall -Wextra ex3_combined.c -o ex3_combined
 */
#include <stdio.h>

int add(int a, int b)
{
    return a + b;
}

int main(void)
{
    printf("3 + 4 = %d\n", add(3, 4));
    return 0;
}
