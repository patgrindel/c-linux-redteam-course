/* Exercise 2 — a safe bounded copy written by hand.
   The guard "i + 1 < dstsize" always leaves one byte for the terminator, so
   the result is never unterminated and never overflows. */
#include <stdio.h>

void safe_copy(char *dst, size_t dstsize, const char *src) {
    if (dstsize == 0) {
        return;                     /* no room even for a NUL */
    }
    size_t i = 0;
    while (i + 1 < dstsize && src[i] != '\0') {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';                  /* always terminate */
}

int main(void) {
    char buf[8];
    safe_copy(buf, sizeof buf, "supercalifragilistic");
    printf("\"%s\"\n", buf);        /* expect "superca" */

    safe_copy(buf, sizeof buf, "hi");
    printf("\"%s\"\n", buf);        /* expect "hi" */
    return 0;
}
