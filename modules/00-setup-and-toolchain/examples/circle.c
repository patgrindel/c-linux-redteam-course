/* circle.c - a program that uses the C preprocessor.
 *
 * Build:  gcc -std=c11 -Wall -Wextra circle.c -o circle
 * Run:    ./circle
 *
 * See the preprocessor's work on its own:
 *   gcc -E circle.c | tail -n 20
 * Notice PI is literally replaced by its value before the compiler runs.
 */
#include <stdio.h>

#define PI 3.14159265358979   /* a preprocessor macro: pure text substitution */

int main(void)
{
    double radius = 2.0;
    double area = PI * radius * radius;   /* PI becomes the number at compile time */
    printf("area of a circle with radius %.1f is %.4f\n", radius, area);
    return 0;
}
