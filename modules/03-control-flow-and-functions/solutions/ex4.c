/* ex4.c - return both min and max through output pointers.
 *
 * Approach: min_max returns nothing; it writes the smallest of a, b, c into *lo
 * and the largest into *hi. We start by assuming a is both, then compare b and c
 * against the running results. The caller passes &min and &max to receive them.
 */
#include <stdio.h>

static void min_max(int a, int b, int c, int *lo, int *hi)
{
    int smallest = a;
    int largest = a;

    if (b < smallest) smallest = b;
    if (c < smallest) smallest = c;
    if (b > largest)  largest = b;
    if (c > largest)  largest = c;

    *lo = smallest;
    *hi = largest;
}

int main(void)
{
    int min;
    int max;

    min_max(7, 2, 5, &min, &max);
    printf("min = %d, max = %d\n", min, max);
    return 0;
}
