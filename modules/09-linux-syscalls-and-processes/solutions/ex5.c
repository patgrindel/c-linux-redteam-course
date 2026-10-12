/* ex5.c - list running PIDs and process names from /proc.
 *
 * Approach: iterate /proc with opendir/readdir; a numeric entry is a PID; read
 * the name from /proc/<pid>/comm. Processes can vanish mid-scan, so a failed
 * fopen is skipped, not fatal. This is the capstone's process-enumeration core.
 *
 * Build: gcc -std=c11 -Wall -Wextra ex5.c -o ex5
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

        char path[64];
        snprintf(path, sizeof path, "/proc/%s/comm", e->d_name);
        FILE *f = fopen(path, "r");
        if (f == NULL) {
            continue;                        /* process exited between listing and now */
        }
        char name[128] = "?";
        if (fgets(name, sizeof name, f) != NULL) {
            name[strcspn(name, "\n")] = '\0';
        }
        fclose(f);
        printf("%6s %s\n", e->d_name, name);
    }

    closedir(d);
    return 0;
}
