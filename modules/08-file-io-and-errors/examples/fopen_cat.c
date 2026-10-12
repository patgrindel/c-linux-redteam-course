/* fopen_cat.c - a tiny `cat` using buffered streams (FILE *).
 *
 * Build: gcc -std=c11 -Wall -Wextra fopen_cat.c -o fopen_cat
 * Run:   ./fopen_cat <file>
 */
#include <stdio.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (f == NULL) {                 /* ALWAYS check fopen */
        perror(argv[1]);
        return 1;
    }

    char line[256];
    /* fgets keeps the trailing '\n'; returns NULL at EOF or on error. */
    while (fgets(line, sizeof line, f) != NULL) {
        fputs(line, stdout);
    }

    if (ferror(f)) {                 /* clean EOF vs. read error */
        fprintf(stderr, "error reading %s\n", argv[1]);
        fclose(f);
        return 1;
    }

    fclose(f);
    return 0;
}
