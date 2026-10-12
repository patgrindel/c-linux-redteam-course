#include <stdio.h>
#include <string.h>

/* The SAFE way to read a line: fgets takes the buffer size and never writes
   past it. Compare to the infamous gets(), which had no size argument, could
   not be used safely, and was REMOVED from C11 for that reason. */
int main(void) {
    char buf[16];

    printf("type something and press enter:\n");
    if (fgets(buf, sizeof buf, stdin) == NULL) {
        printf("no input\n");
        return 0;
    }

    /* fgets keeps the trailing '\n' if it fit. strcspn finds the newline (or
       the NUL if there is none); overwrite it with a terminator. */
    buf[strcspn(buf, "\n")] = '\0';

    printf("you typed %zu chars: \"%s\"\n", strlen(buf), buf);
    printf("(anything past %zu chars was safely left in the buffer)\n",
           sizeof buf - 1);
    return 0;
}
