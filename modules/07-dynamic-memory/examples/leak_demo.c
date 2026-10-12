#include <stdio.h>
#include <stdlib.h>

/* This program allocates and never frees -- a memory leak. It runs and exits
   normally; the leak is invisible without a tool. Run it under valgrind
   (see the module README) to watch the leak be reported. */
int main(void) {
    int *data = malloc(100 * sizeof *data);
    if (data == NULL) {
        return 1;
    }
    data[0] = 42;
    printf("data[0] = %d\n", data[0]);

    /* BUG: we return without free(data). The 400 bytes are leaked. */
    return 0;
}
