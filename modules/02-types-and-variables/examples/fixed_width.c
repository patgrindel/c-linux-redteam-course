/* fixed_width.c - exact-size integers from <stdint.h>.
 *
 * Build:  gcc -std=c11 -Wall -Wextra fixed_width.c -o fixed_width
 * Run:    ./fixed_width
 *
 * When the size of an integer matters - a network packet field, a file-format
 * offset, a hardware register, anything where "4 bytes exactly" is part of the
 * contract - use these instead of int/long. The number in the name is the width
 * in bits. To print them portably, use the macros from <inttypes.h> (PRId32,
 * PRIu8, ...), which expand to the right length modifier for this platform.
 */
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

int main(void)
{
    uint8_t  byte    = 255;            /* 0 .. 255                      */
    int16_t  small   = -30000;         /* -32768 .. 32767               */
    uint32_t medium  = 4000000000U;    /* 0 .. ~4.29 billion            */
    int64_t  big     = -9000000000LL;  /* huge signed range             */

    printf("uint8_t  byte   = %" PRIu8  "  (%zu bytes)\n", byte,   sizeof byte);
    printf("int16_t  small  = %" PRId16 "  (%zu bytes)\n", small,  sizeof small);
    printf("uint32_t medium = %" PRIu32 "  (%zu bytes)\n", medium, sizeof medium);
    printf("int64_t  big    = %" PRId64 "  (%zu bytes)\n", big,    sizeof big);
    return 0;
}
