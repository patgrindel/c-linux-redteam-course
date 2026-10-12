/* static_typing.c - in C, every variable has one fixed type, known at compile time.
 *
 * Build:  gcc -std=c11 -Wall -Wextra static_typing.c -o static_typing
 * Run:    ./static_typing
 *
 * In Python a name can hold anything and be rebound to another type at will.
 * In C a variable's type is declared once and never changes. The compiler knows
 * each variable's size and layout before the program ever runs. sizeof reports
 * that size in bytes (sizeof's result has type size_t, printed with %zu).
 */
#include <stdio.h>

int main(void)
{
    int   count   = 42;      /* an integer, typically 4 bytes */
    double price  = 9.99;    /* a double-precision float, typically 8 bytes */
    char  initial = 'C';     /* a single character, 1 byte, really a small int */

    printf("count   = %d    (%zu bytes)\n", count, sizeof count);
    printf("price   = %.2f  (%zu bytes)\n", price, sizeof price);
    printf("initial = %c     (%zu bytes)\n", initial, sizeof initial);

    /* 'count' is forever an int. You cannot later write count = "hello";
     * the compiler would reject it. Try it and read the error. */
    return 0;
}
