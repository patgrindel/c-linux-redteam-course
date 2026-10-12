/* functions.c - declaring, defining, and calling functions; pass-by-value.
 *
 * Build:  gcc -std=c11 -Wall -Wextra functions.c -o functions
 * Run:    ./functions
 *
 * The key C fact: arguments are passed BY VALUE. The function receives a COPY of
 * each argument. Assigning to a parameter changes only that copy, never the
 * caller's variable. (To let a function change a caller's variable you pass a
 * pointer - see multi_return.c and Module 04.)
 */
#include <stdio.h>

/* prototypes */
int  max2(int a, int b);
void try_to_change(int x);

int main(void)
{
    printf("max2(3, 9) = %d\n", max2(3, 9));

    int n = 5;
    printf("before: n = %d\n", n);
    try_to_change(n);                 /* passes a COPY of n */
    printf("after:  n = %d  (unchanged - pass-by-value)\n", n);
    return 0;
}

int max2(int a, int b)
{
    return (a > b) ? a : b;           /* the ternary ?: - a compact if/else */
}

void try_to_change(int x)
{
    x = 999;                          /* modifies the local copy only */
    printf("inside: x = %d\n", x);
}
