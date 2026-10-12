/* ex1.c - report size and maximum of several fixed-width types.
 *
 * Approach: sizeof gives the byte size (printed with %zu); the UINTn_MAX /
 * INT32_MAX macros from <stdint.h> give the maxima without hard-coding numbers.
 * The PRI* macros from <inttypes.h> give the correct printf conversion for each
 * fixed-width type.
 */
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

int main(void)
{
    printf("uint8_t : %zu byte(s), max %" PRIu8  "\n", sizeof(uint8_t),  UINT8_MAX);
    printf("uint16_t: %zu byte(s), max %" PRIu16 "\n", sizeof(uint16_t), UINT16_MAX);
    printf("uint32_t: %zu byte(s), max %" PRIu32 "\n", sizeof(uint32_t), UINT32_MAX);
    printf("int32_t : %zu byte(s), max %" PRId32 "\n", sizeof(int32_t),  INT32_MAX);
    return 0;
}
