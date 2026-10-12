#include <stdio.h>
#include <stdlib.h>

/* malloc reserves a block of bytes on the heap and returns a pointer to it,
   or NULL if it failed. The memory lives until YOU call free. */
int main(void) {
    size_t n = 5;
    int *arr = malloc(n * sizeof *arr);   /* sizeof *arr == sizeof(int) */
    if (arr == NULL) {                    /* ALWAYS check malloc */
        perror("malloc");
        return 1;
    }

    for (size_t i = 0; i < n; i++) {
        arr[i] = (int)(i * i);
    }
    for (size_t i = 0; i < n; i++) {
        printf("arr[%zu] = %d\n", i, arr[i]);
    }

    free(arr);     /* hand the memory back exactly once */
    return 0;
}
