#include <stdio.h>

/* A C string is just a char array whose end is marked by a '\0' (NUL) byte.
   There is no separate "string" type and no stored length -- the NUL is the
   only thing that says "stop". */
int main(void) {
    char word[6] = {'h', 'e', 'l', 'l', 'o', '\0'};
    char same[] = "hello";   /* the compiler adds the '\0' for you -> 6 bytes */

    printf("word = %s\n", word);
    printf("same = %s\n", same);
    printf("sizeof the \"hello\" array = %zu (5 letters + 1 NUL)\n", sizeof same);

    /* Walk the bytes so you can SEE the terminator. */
    for (size_t i = 0; i < sizeof same; i++) {
        if (same[i] == '\0') {
            printf("[%zu]=\\0 ", i);
        } else {
            printf("[%zu]=%c ", i, same[i]);
        }
    }
    putchar('\n');
    return 0;
}
