/* ex3.c - the caller (Exercise 3 solution).
 *
 * Approach: this file holds only main. It includes mathutil.h for the
 * declaration of add() and relies on the linker to connect the call to the
 * definition in mathutil.c.
 *
 * Build:  gcc -std=c11 -Wall -Wextra ex3.c mathutil.c -o ex3
 *
 * Omit mathutil.c and the LINKER (not the compiler) fails with
 * "undefined reference to `add'".
 */
#include <stdio.h>
#include "mathutil.h"

int main(void)
{
    printf("3 + 4 = %d\n", add(3, 4));
    return 0;
}
