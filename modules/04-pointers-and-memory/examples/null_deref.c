#include <stdio.h>

/* A NULL pointer points at "nothing". Dereferencing it is a bug that
   the OS catches: your process gets a SIGSEGV and dies. Run it and watch. */
int main(void) {
    int *p = NULL;

    printf("p holds the address: %p\n", (void *)p);
    printf("about to dereference NULL...\n");
    fflush(stdout);         /* force the message out before we crash */

    int value = *p;         /* CRASH: reading through a NULL pointer */
    printf("you will never see this line: %d\n", value);
    return 0;
}
