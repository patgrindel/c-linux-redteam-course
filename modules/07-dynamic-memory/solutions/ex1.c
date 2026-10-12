/* Exercise 1 — read an unknown number of ints, print them reversed.
   The buffer doubles whenever it fills, so we never need to know the count
   up front. The reverse loop uses the i-- > 0 trick so it works with the
   unsigned type size_t without underflowing. */
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int   *arr = NULL;
    size_t len = 0;
    size_t cap = 0;
    int    x;

    while (scanf("%d", &x) == 1) {
        if (len == cap) {
            size_t new_cap = (cap == 0) ? 4 : cap * 2;
            int *tmp = realloc(arr, new_cap * sizeof *arr);
            if (tmp == NULL) {
                perror("realloc");
                free(arr);
                return 1;
            }
            arr = tmp;
            cap = new_cap;
        }
        arr[len++] = x;
    }

    for (size_t i = len; i-- > 0; ) {
        printf("%d\n", arr[i]);
    }

    free(arr);
    return 0;
}
