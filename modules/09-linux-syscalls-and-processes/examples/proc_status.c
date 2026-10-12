/* proc_status.c - print selected fields from /proc/<pid>/status.
 *
 * Defaults to /proc/self (this process). Each line is "Key:\tvalue".
 *
 * Build: gcc -std=c11 -Wall -Wextra proc_status.c -o proc_status
 * Run:   ./proc_status [pid]
 */
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {
    char path[64];
    if (argc == 2) {
        snprintf(path, sizeof path, "/proc/%s/status", argv[1]);
    } else {
        snprintf(path, sizeof path, "/proc/self/status");
    }

    FILE *f = fopen(path, "r");
    if (f == NULL) {
        perror(path);
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof line, f) != NULL) {
        if (strncmp(line, "Name:", 5) == 0 ||
            strncmp(line, "Pid:", 4) == 0 ||
            strncmp(line, "PPid:", 5) == 0 ||
            strncmp(line, "Uid:", 4) == 0 ||
            strncmp(line, "VmRSS:", 6) == 0) {
            fputs(line, stdout);
        }
    }
    fclose(f);
    return 0;
}
