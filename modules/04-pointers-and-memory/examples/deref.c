#include <stdio.h>

/* * ("dereference") follows a pointer to the thing it points at.
   You can read through it and write through it. */
int main(void) {
    int x = 42;
    int *p = &x;

    printf("x via x : %d\n", x);
    printf("x via *p: %d\n", *p);   /* read through the pointer */

    *p = 99;                        /* write through the pointer */
    printf("after *p = 99, x = %d\n", x);
    return 0;
}
