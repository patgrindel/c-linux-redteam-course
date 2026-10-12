/* ex3_starter.c - fill in the flag operations (Exercise 3).
 *
 * The helper and macros are provided. Replace the TODO section so the program
 * sets READ|WRITE, reports whether each flag is present, toggles EXEC on, and
 * clears WRITE - printing the binary after each change.
 *
 * As given, this compiles and runs (it just doesn't do the work yet):
 *   gcc -std=c11 -Wall -Wextra ex3_starter.c -o ex3
 */
#include <stdio.h>
#include <stdint.h>

static void print_bits_u8(uint8_t v)
{
    for (int i = 7; i >= 0; i--) {
        putchar(((v >> i) & 1u) ? '1' : '0');
    }
    putchar('\n');
}

#define FLAG_READ  (1u << 0)
#define FLAG_WRITE (1u << 1)
#define FLAG_EXEC  (1u << 2)

int main(void)
{
    uint8_t perms = 0;

    /* TODO: set READ and WRITE, then print_bits_u8(perms);               */
    /* TODO: print whether READ, WRITE, EXEC are present (as 0/1);        */
    /* TODO: toggle EXEC on, then print_bits_u8(perms);                   */
    /* TODO: clear WRITE, then print_bits_u8(perms);                      */

    print_bits_u8(perms);   /* placeholder so the starter does something */
    return 0;
}
