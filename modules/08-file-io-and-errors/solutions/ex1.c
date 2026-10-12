/* ex1.c - line counter.
 *
 * Approach: read one character at a time with fgetc and count newlines. Simple
 * and exact; a file not ending in '\n' still counts its preceding lines
 * correctly (we count terminators, matching `wc -l`).
 *
 * Build: gcc -std=c11 -Wall -Wextra ex1.c -o ex1
 */
#include <stdio.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {
        perror(argv[1]);
        return 1;
    }

    long lines = 0;
    int c;
    while ((c = fgetc(f)) != EOF) {
        if (c == '\n') {
            lines++;
        }
    }

    if (ferror(f)) {
        fprintf(stderr, "error reading %s\n", argv[1]);
        fclose(f);
        return 1;
    }

    printf("%ld\n", lines);
    fclose(f);
    return 0;
}
