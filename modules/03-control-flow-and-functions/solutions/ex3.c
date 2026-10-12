/* ex3.c - a swap that works, using pointers.
 *
 * Approach: instead of copies, swap receives the ADDRESSES of the caller's
 * variables. Dereferencing (*pa, *pb) reads and writes the actual variables, so
 * the exchange is visible in main. This is the pass-by-reference idiom built
 * from pass-by-value plus pointers.
 */
#include <stdio.h>

static void swap(int *pa, int *pb)
{
    int tmp = *pa;
    *pa = *pb;
    *pb = tmp;
}

int main(void)
{
    int x = 10;
    int y = 20;

    printf("before: x = %d, y = %d\n", x, y);
    swap(&x, &y);                    /* pass addresses */
    printf("after:  x = %d, y = %d\n", x, y);
    return 0;
}
