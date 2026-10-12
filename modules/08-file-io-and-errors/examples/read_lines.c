/* read_lines.c - read a file line by line with getline(), numbering each line.
 *
 * getline() is POSIX.1-2008, not ISO C11, so we expose it with a feature-test
 * macro; without it, -std=c11 hides the declaration.
 *
 * Build: gcc -std=c11 -Wall -Wextra read_lines.c -o read_lines
 * Run:   ./read_lines <file>
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

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

    char *line = NULL;       /* getline allocates/reallocs this for us */
    size_t cap = 0;          /* current buffer capacity */
    ssize_t len;             /* bytes read, or -1 at EOF/error */
    long n = 0;

    while ((len = getline(&line, &cap, f)) != -1) {
        n++;
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        printf("%4ld: %s\n", n, line);
    }

    free(line);              /* getline's buffer is ours to free, once */
    fclose(f);
    return 0;
}
