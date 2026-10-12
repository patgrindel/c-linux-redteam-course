/* ex2.c - demonstrate well-defined unsigned wraparound.
 *
 * Approach: a uint8_t holds 0..255. Incrementing past 255 wraps to 0 (modulo
 * 256), which is defined behavior for unsigned types. We add 1 ten times from a
 * start of 250 so the output crosses the wrap point.
 */
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

int main(void)
{
    uint8_t counter = 250;
    for (int i = 0; i < 10; i++) {
        printf("%" PRIu8 "\n", counter);
        counter++;   /* at 255, this wraps to 0 */
    }
    return 0;
}
