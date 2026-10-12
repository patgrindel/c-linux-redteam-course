#include <stdio.h>

/* A pointer is a variable whose value is a memory address.
   & ("address-of") gives you the address of a variable. */
int main(void) {
    int x = 42;
    int *p = &x;    /* p now holds the address of x */

    printf("value of x       : %d\n", x);
    printf("address of x     : %p\n", (void *)&x);
    printf("value stored in p: %p\n", (void *)p);
    printf("size of an int   : %zu bytes\n", sizeof x);
    printf("size of p (int *): %zu bytes\n", sizeof p);
    return 0;
}
