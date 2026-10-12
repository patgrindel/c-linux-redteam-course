/* ex3.c - list accounts with a real login shell from /etc/passwd.
 *
 * Approach: reuse the in-place colon splitter (handles empty fields); a small
 * blocklist decides which shells don't count as interactive logins. Flagging
 * who can actually log in is a first-pass host-assessment step.
 *
 * Build: gcc -std=c11 -Wall -Wextra ex3.c -o ex3
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

static int is_login_shell(const char *shell) {
    static const char *nologin[] = {
        "", "/usr/sbin/nologin", "/sbin/nologin", "/bin/false", "/usr/bin/false",
    };
    for (size_t i = 0; i < sizeof nologin / sizeof nologin[0]; i++) {
        if (strcmp(shell, nologin[i]) == 0) {
            return 0;
        }
    }
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
    int lineno = 0;

    while ((len = getline(&line, &cap, f)) != -1) {
        lineno++;
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        char *fields[NFIELDS];
        int n = split_fields(line, fields, NFIELDS);
        if (n != NFIELDS) {
            fprintf(stderr, "%s:%d: malformed line (%d fields)\n", path, lineno, n);
            continue;
        }

        if (is_login_shell(fields[6])) {
            printf("%s  %s\n", fields[0], fields[6]);
        }
    }

    free(line);
    fclose(f);
    return 0;
}
