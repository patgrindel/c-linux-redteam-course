/* multi_return.c - returning more than one value, using output pointers.
 *
 * Build:  gcc -std=c11 -Wall -Wextra multi_return.c -o multi_return
 * Run:    ./multi_return
 *
 * A C function returns a single value. To hand back several, the caller gives
 * the function the ADDRESSES of variables to fill in. '&q' means "the address of
 * q"; inside the function, '*quotient' means "the variable that pointer points
 * at". Writing through the pointer updates the caller's own variable - this is
 * how a function reaches out and changes something in its caller. Pointers get
 * the full treatment in Module 04; this is the motivating preview.
 */
#include <stdio.h>

/* Compute both the quotient and the remainder of a / b. */
static void divmod(int a, int b, int *quotient, int *remainder)
{
    *quotient  = a / b;   /* write the result into the caller's variable */
    *remainder = a % b;
}

int main(void)
{
    int q;
    int r;

    divmod(17, 5, &q, &r);   /* pass the addresses of q and r */

    printf("17 / 5 = %d remainder %d\n", q, r);
    return 0;
}
