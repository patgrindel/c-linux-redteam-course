/* ex3_starter.c - a swap that DOESN'T work. Fix it with pointers (Exercise 3).
 *
 * Build and run it first to see the problem:
 *   gcc -std=c11 -Wall -Wextra ex3_starter.c -o ex3_starter
 *   ./ex3_starter
 *
 * swap() receives COPIES of a and b, exchanges the copies, and returns. The
 * caller's x and y never change. Your job: change swap to take int* and operate
 * through the pointers, and update main to pass &x and &y.
 */
#include <stdio.h>

void swap(int a, int b)
{
    int tmp = a;
    a = b;
    b = tmp;        /* only the local copies are swapped */
}

int main(void)
{
    int x = 10;
    int y = 20;

    printf("before: x = %d, y = %d\n", x, y);
    swap(x, y);
    printf("after:  x = %d, y = %d  (unchanged - that's the bug)\n", x, y);
    return 0;
}
