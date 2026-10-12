/* ex3.c - permission flags with the bitwise idioms.
 *
 * Approach: each permission is one bit. OR (|=) sets bits, AND with a mask (&)
 * tests them, XOR (^=) toggles, and AND with the inverted mask (&= ~) clears.
 * Casting ~FLAG_WRITE back to uint8_t keeps the value in one byte cleanly.
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

    perms |= FLAG_READ | FLAG_WRITE;        /* set */
    print_bits_u8(perms);

    printf("READ?  %d\n", (perms & FLAG_READ)  != 0);
    printf("WRITE? %d\n", (perms & FLAG_WRITE) != 0);
    printf("EXEC?  %d\n", (perms & FLAG_EXEC)  != 0);

    perms ^= FLAG_EXEC;                      /* toggle EXEC on */
    print_bits_u8(perms);

    perms &= (uint8_t)~FLAG_WRITE;           /* clear WRITE */
    print_bits_u8(perms);
    return 0;
}
