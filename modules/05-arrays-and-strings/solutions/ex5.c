/* Exercise 5 — parse a "-n COUNT" flag and repeat the last argument.
   Shows the walk-argv / match-with-strcmp / consume-the-value pattern. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    int count = 1;
    const char *text = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            count = atoi(argv[++i]);   /* consume the value */
            if (count < 1) {
                count = 1;             /* ignore junk / non-positive input */
            }
        } else {
            text = argv[i];            /* last non-flag arg wins */
        }
    }

    if (text == NULL) {
        fprintf(stderr, "usage: %s [-n COUNT] TEXT\n", argv[0]);
        return 1;
    }

    for (int i = 0; i < count; i++) {
        printf("%s\n", text);
    }
    return 0;
}
