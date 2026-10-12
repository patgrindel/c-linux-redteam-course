#include <stdio.h>
#include <stdlib.h>

/* realloc resizes an existing block, copying the old contents. The safe idiom
   is to assign its result to a TEMP pointer: on failure realloc returns NULL
   but the old block is still valid, so writing straight to arr would leak it. */
int main(void) {
    size_t n = 2;
    int *arr = malloc(n * sizeof *arr);
    if (arr == NULL) {
        perror("malloc");
        return 1;
    }
    arr[0] = 1;
    arr[1] = 2;

    size_t new_n = 4;
    int *tmp = realloc(arr, new_n * sizeof *arr);
    if (tmp == NULL) {
        perror("realloc");
        free(arr);          /* old block is still ours to free */
        return 1;
    }
    arr = tmp;              /* only now is it safe to overwrite arr */
    arr[2] = 3;
    arr[3] = 4;

    for (size_t i = 0; i < new_n; i++) {
        printf("arr[%zu] = %d\n", i, arr[i]);
    }

    free(arr);
    return 0;
}
