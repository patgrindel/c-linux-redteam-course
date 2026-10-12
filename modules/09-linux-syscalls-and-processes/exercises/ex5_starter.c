/* ex5_starter.c - list running PIDs and their names from /proc.
 *
 * Build: gcc -std=c11 -Wall -Wextra ex5_starter.c -o ex5
 * Run:   ./ex5
 */
#define _POSIX_C_SOURCE 200809L
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

static int all_digits(const char *s) {
    if (*s == '\0') {
        return 0;
    }
    for (; *s; s++) {
        if (!isdigit((unsigned char) *s)) {
            return 0;
        }
    }
    return 1;
}

int main(void) {
    DIR *d = opendir("/proc");
    if (d == NULL) {
        perror("/proc");
        return 1;
    }

    struct dirent *e;
    while ((e = readdir(d)) != NULL) {
        if (!all_digits(e->d_name)) {
            continue;
        }
        /* TODO: build "/proc/<pid>/comm", fopen it (skip on failure),
                 fgets one line, strip '\n', and print "%6s %s". */
    }

    closedir(d);
    return 0;
}
