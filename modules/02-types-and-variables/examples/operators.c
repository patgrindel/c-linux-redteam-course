/* operators.c - arithmetic, division/modulo, and precedence.
 *
 * Build:  gcc -std=c11 -Wall -Wextra operators.c -o operators
 * Run:    ./operators
 *
 * The big surprise for Python programmers: between two integers, '/' is integer
 * division (it throws away the remainder), and there is no separate '//'. If you
 * want a fractional result, at least one operand must be a floating type.
 */
#include <stdio.h>

int main(void)
{
    printf("7 / 2    = %d   (integer division: quotient only)\n", 7 / 2);
    printf("7 %% 2    = %d   (modulo: the remainder)\n", 7 % 2);
    printf("7.0 / 2  = %g (floating division: one operand is a double)\n", 7.0 / 2);

    /* Precedence: * and / bind tighter than + and -, just like in math. */
    printf("\n2 + 3 * 4   = %d   (* first)\n", 2 + 3 * 4);
    printf("(2 + 3) * 4 = %d   (parentheses first)\n", (2 + 3) * 4);

    /* Comparison yields 1 (true) or 0 (false); logical && and || short-circuit. */
    int x = 5;
    printf("\n(x > 0) is %d\n", x > 0);
    printf("(x > 0 && x < 10) is %d\n", x > 0 && x < 10);
    printf("!(x == 5) is %d\n", !(x == 5));
    return 0;
}
