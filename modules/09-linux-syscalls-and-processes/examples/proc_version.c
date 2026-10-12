/* proc_version.c - read the kernel version string from /proc/version.
 *
 * /proc files are plain text; ordinary stdio works. No feature-test macro needed.
 *
 * Build: gcc -std=c11 -Wall -Wextra proc_version.c -o proc_version
 * Run:   ./proc_version
 */
#include <stdio.h>

int main(void) {
    FILE *f = fopen("/proc/version", "r");
    if (f == NULL) {
        perror("/proc/version");
        return 1;
    }
    char line[512];
    if (fgets(line, sizeof line, f) != NULL) {
        fputs(line, stdout);
    }
    fclose(f);
    return 0;
}
