/* ex3_starter.c - builds, but not cleanly. Fix the warnings.
 *
 * Try:  gcc -std=c11 -Wall -Wextra ex3_starter.c -o ex3
 * and read what gcc complains about.
 */
#include <stdio.h>

int main(int argc, char **argv)
{
    int total;            /* problem 1: used before it is set */
    total = total + 10;

    printf("total is %d\n", total);
    return 0;
}
