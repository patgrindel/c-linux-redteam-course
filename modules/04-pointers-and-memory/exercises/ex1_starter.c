#include <stdio.h>

/* TODO: write increment() so it adds 1 to the int that p points at. */
void increment(int *p) {
    (void)p;   /* delete this line once you use p */
}

int main(void) {
    int x = 10;
    printf("x = %d at address %p\n", x, (void *)&x);

    increment(&x);

    printf("x = %d\n", x);   /* should print 11 */
    return 0;
}
