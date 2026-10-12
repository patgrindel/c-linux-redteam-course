#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* The free happens inside destroy(), so after the call `msg` is a dangling
   pointer: it still holds the old address, but that memory is no longer ours.
   Touching it is use-after-free -- undefined behavior. It may print garbage,
   print the old text, or crash. valgrind reports it precisely. */
static void destroy(char *p) {
    free(p);
}

int main(void) {
    char *msg = malloc(16);
    if (msg == NULL) {
        return 1;
    }
    strcpy(msg, "hello");
    destroy(msg);                 /* msg is now dangling */

    printf("%s\n", msg);          /* BUG: use after free */
    return 0;
}
