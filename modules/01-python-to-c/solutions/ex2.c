/* ex2.c - prototype above main, definition below.
 *
 * Approach: the compiler reads top to bottom, so to call cube() before its body
 * appears we supply a prototype first. Without the prototype the compiler would
 * not know cube's return type or parameters and the build would fail.
 */
#include <stdio.h>

int cube(int n);   /* prototype */

int main(void)
{
    printf("2 cubed is %d\n", cube(2));
    printf("3 cubed is %d\n", cube(3));
    return 0;
}

int cube(int n)    /* definition, after main */
{
    return n * n * n;
}
