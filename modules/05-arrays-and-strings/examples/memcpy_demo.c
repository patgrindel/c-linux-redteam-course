#include <stdio.h>
#include <string.h>

/* memcpy copies raw BYTES and knows nothing about NUL terminators. Use it for
   any fixed-size block (arrays, structs). The regions must not overlap --
   use memmove if they might. */
int main(void) {
    int src[5] = {1, 2, 3, 4, 5};
    int dst[5];

    memcpy(dst, src, sizeof src);   /* copy all 5 ints = 20 bytes */

    for (size_t i = 0; i < 5; i++) {
        printf("%d ", dst[i]);
    }
    putchar('\n');
    return 0;
}
