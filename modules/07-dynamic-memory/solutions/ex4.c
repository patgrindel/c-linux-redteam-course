/* Exercise 4 — deep-copy an int array.
   The returned block is independent of src, so later edits to src do not touch
   the copy. The caller owns the copy and frees it. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int *copy_array(const int *src, size_t n) {
    int *dst = malloc(n * sizeof *dst);
    if (dst == NULL) {
        return NULL;
    }
    memcpy(dst, src, n * sizeof *dst);
    return dst;
}

int main(void) {
    int src[3] = {1, 2, 3};
    int *copy = copy_array(src, 3);
    if (copy == NULL) {
        perror("copy_array");
        return 1;
    }

    src[0] = 99;   /* mutate the original AFTER copying */
    printf("src[0]  = %d\n", src[0]);      /* 99 */
    printf("copy[0] = %d\n", copy[0]);     /* still 1 -> it is a real copy */

    free(copy);
    return 0;
}
