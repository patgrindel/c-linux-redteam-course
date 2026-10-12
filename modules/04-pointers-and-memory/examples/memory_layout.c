#include <stdio.h>
#include <stdlib.h>

int global_initialized = 1;   /* lives in the data segment  */
int global_uninitialized;     /* lives in the bss  segment  */

static void a_function(void) {
    /* Here only so we can take its address and see where code lives. */
}

int main(void) {
    int local = 0;                        /* stack */
    int *heap = malloc(sizeof *heap);     /* heap  */
    if (heap == NULL) {
        perror("malloc");
        return 1;
    }
    *heap = 7;

    /* Print one address from each region. The exact numbers differ every
       run and on every machine, but notice the RELATIVE ordering. */
    printf("text   (code)   a_function          : %p\n", (void *)a_function);
    printf("data   (init)   global_initialized  : %p\n", (void *)&global_initialized);
    printf("bss    (uninit) global_uninitialized: %p\n", (void *)&global_uninitialized);
    printf("heap            *heap               : %p\n", (void *)heap);
    printf("stack           local               : %p\n", (void *)&local);

    free(heap);
    return 0;
}
