/* ex2.c - grep-lite: print lines of a file containing a substring.
 *
 * Approach: getline so long lines aren't truncated; strstr for the match;
 * strip the trailing newline so our numbered output is tidy.
 *
 * Build: gcc -std=c11 -Wall -Wextra ex2.c -o ex2
 * Run:   ./ex2 <pattern> <file>
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <pattern> <file>\n", argv[0]);
        return 1;
    }
    const char *pattern = argv[1];

    FILE *f = fopen(argv[2], "r");
    if (f == NULL) {
        perror(argv[2]);
        return 1;
    }

    char *line = NULL;
    size_t cap = 0;
    ssize_t len;
    long n = 0;

    while ((len = getline(&line, &cap, f)) != -1) {
        n++;
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        if (strstr(line, pattern) != NULL) {
            printf("%ld: %s\n", n, line);
        }
    }

    free(line);
    fclose(f);
    return 0;
}
