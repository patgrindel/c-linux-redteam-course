/* parse_passwd.c - read /etc/passwd line by line and report login accounts.
 *
 * Each line has 7 colon-separated fields:
 *   name:passwd:uid:gid:gecos:home:shell
 * Fields may be empty (e.g. gecos), producing "::", so we split by hand with
 * a scan for ':' instead of strtok() (which collapses adjacent delimiters).
 *
 * This is the parsing core reused by the capstone's user-enumeration check.
 *
 * Build: gcc -std=c11 -Wall -Wextra parse_passwd.c -o parse_passwd
 * Run:   ./parse_passwd [file]     (defaults to /etc/passwd)
 */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NFIELDS 7

/* Split `line` in place on ':' into up to `max` pieces. Writes pointers into
 * out[]; returns the field count. Replaces each ':' with '\0'. */
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
            continue;                       /* skip blanks and comments */
        }

        char *fields[NFIELDS];
        int n = split_fields(line, fields, NFIELDS);
        if (n != NFIELDS) {
            fprintf(stderr, "%s:%d: expected %d fields, got %d (skipping)\n",
                    path, lineno, NFIELDS, n);
            continue;
        }

        /* fields: 0=name 1=passwd 2=uid 3=gid 4=gecos 5=home 6=shell */
        char *end;
        long uid = strtol(fields[2], &end, 10);
        if (*end != '\0') {
            fprintf(stderr, "%s:%d: bad UID '%s'\n", path, lineno, fields[2]);
            continue;
        }

        /* Report root and "real" users (UID >= 1000) for host triage. */
        if (uid == 0 || uid >= 1000) {
            printf("%-16s uid=%-6ld shell=%s\n", fields[0], uid, fields[6]);
        }
    }

    free(line);
    fclose(f);
    return 0;
}
