/* ex3_starter.c - list users with a real login shell from /etc/passwd.
 *
 * Build: gcc -std=c11 -Wall -Wextra ex3_starter.c -o ex3
 * Run:   ./ex3 [file]
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NFIELDS 7

static int split_fields(char *line, char *out[], int max) {
    int n = 0;
    char *start = line;
    for (char *p = line; ; p++) {
        if (*p == ':' || *p == '\0') {
            int last = (*p == '\0');
            *p = '\0';
            if (n < max) {
                out[n++] = start;
            }
            start = p + 1;
            if (last) {
                break;
            }
        }
    }
    return n;
}

/* TODO: return 1 if `shell` is a real login shell, 0 otherwise.
 * Treat "", "/usr/sbin/nologin", "/sbin/nologin", "/bin/false" as NOT login. */
static int is_login_shell(const char *shell) {
    (void) shell;
    return 1;
}

int main(int argc, char **argv) {
    const char *path = (argc == 2) ? argv[1] : "/etc/passwd";
    FILE *f = fopen(path, "r");
    if (f == NULL) {
        perror(path);
        return 1;
    }

    char *line = NULL;
    size_t cap = 0;
    ssize_t len;
    /* TODO: loop with getline; split; if is_login_shell(fields[6]) print name+shell. */
    (void) len;

    free(line);
    fclose(f);
    return 0;
}
