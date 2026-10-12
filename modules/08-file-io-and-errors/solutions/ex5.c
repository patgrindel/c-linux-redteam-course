/* ex5.c - key=value config parser.
 *
 * Approach: read lines with getline; skip blanks and '#' comments; split on the
 * FIRST '='; trim spaces from both sides in place; report lines with no '='.
 *
 * Build: gcc -std=c11 -Wall -Wextra ex5.c -o ex5
 * Run:   ./ex5 <file>
 */
#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Trim leading/trailing whitespace in place; returns pointer to first non-space. */
static char *trim(char *s) {
    while (*s && isspace((unsigned char) *s)) {
        s++;
    }
    char *end = s + strlen(s);
    while (end > s && isspace((unsigned char) end[-1])) {
        *--end = '\0';
    }
    return s;
}

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

    char *line = NULL;
    size_t cap = 0;
    ssize_t len;
    int lineno = 0;

    while ((len = getline(&line, &cap, f)) != -1) {
        lineno++;
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        char *s = trim(line);
        if (s[0] == '\0' || s[0] == '#') {
            continue;
        }

        char *eq = strchr(s, '=');     /* first '=' only */
        if (eq == NULL) {
            fprintf(stderr, "%s:%d: no '=' found, skipping\n", argv[1], lineno);
            continue;
        }

        *eq = '\0';
        char *key = trim(s);
        char *val = trim(eq + 1);
        printf("%s=%s\n", key, val);
    }

    free(line);
    fclose(f);
    return 0;
}
