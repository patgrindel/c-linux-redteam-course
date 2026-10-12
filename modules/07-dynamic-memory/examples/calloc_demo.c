#include <stdio.h>
#include <stdlib.h>

/* calloc(count, size) allocates count*size bytes AND zeroes them. malloc
   leaves the bytes uninitialized (garbage). Reach for calloc when you want a
   clean, zeroed start -- and it also guards against count*size overflow. */
int main(void) {
    size_t n = 5;
    int *arr = calloc(n, sizeof *arr);
    if (arr == NULL) {
        perror("calloc");
        return 1;
    }

    for (size_t i = 0; i < n; i++) {
        printf("arr[%zu] = %d\n", i, arr[i]);   /* every element is 0 */
    }

    free(arr);
    return 0;
}
