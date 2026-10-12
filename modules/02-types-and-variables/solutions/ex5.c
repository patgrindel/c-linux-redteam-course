/* ex5.c - read a number 0..255 and print it in 8-bit binary.
 *
 * Approach: parse the input with the fgets + strtol pattern and reject anything
 * outside 0..255. Then walk the bits from most-significant (bit 7) to least,
 * shifting the value right by i and masking the low bit with & 1u. This is the
 * bitwise way to extract a single bit, rather than repeated division.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

int main(void)
{
    char line[64];
    if (fgets(line, sizeof line, stdin) == NULL) {
        fprintf(stderr, "no input\n");
        return 1;
    }

    errno = 0;
    char *end;
    long value = strtol(line, &end, 10);
    if (end == line || errno == ERANGE || value < 0 || value > 255) {
        fprintf(stderr, "please enter a number from 0 to 255\n");
        return 1;
    }

    uint8_t byte = (uint8_t)value;
    for (int i = 7; i >= 0; i--) {
        putchar(((byte >> i) & 1u) ? '1' : '0');
    }
    putchar('\n');
    return 0;
}
