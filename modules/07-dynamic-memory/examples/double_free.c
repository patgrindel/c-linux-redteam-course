#include <stdio.h>
#include <stdlib.h>

/* Freeing the same block twice corrupts the allocator's bookkeeping. It often
   crashes, and is a classic exploitable bug. The fix is to have ONE owner and
   free exactly once -- and to set pointers to NULL after freeing, since
   free(NULL) is harmless. */
static void destroy(int *p) {
    free(p);
}

int main(void) {
    int *data = malloc(sizeof *data);
    if (data == NULL) {
        return 1;
    }
    *data = 7;
    printf("data = %d\n", *data);

    destroy(data);
    destroy(data);   /* BUG: double free */
    return 0;
}
