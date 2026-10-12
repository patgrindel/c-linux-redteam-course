/* ex4.c - summarize this process from /proc/self/status.
 *
 * Approach: read the status file line by line; for the keys we care about, skip
 * past the ':' and any leading whitespace, then print "key = value".
 *
 * Build: gcc -std=c11 -Wall -Wextra ex4.c -o ex4
 * Run:   ./ex4
 */
#include <ctype.h>
#include <stdio.h>
#include <string.h>

static void print_field(const char *line, const char *key) {
    size_t klen = strlen(key);
    const char *v = line + klen;             /* skip "Key:" */
    while (*v == ' ' || *v == '\t') {
        v++;
    }
    /* copy up to newline */
    char val[128];
    size_t i = 0;
    while (v[i] && v[i] != '\n' && i < sizeof val - 1) {
        val[i] = v[i];
        i++;
    }
    val[i] = '\0';
    /* key without the trailing ':' */
    char name[32];
    size_t n = (klen - 1 < sizeof name - 1) ? klen - 1 : sizeof name - 1;
    memcpy(name, key, n);
    name[n] = '\0';
    printf("%s = %s\n", name, val);
}

int main(void) {
    FILE *f = fopen("/proc/self/status", "r");
    if (f == NULL) {
        perror("/proc/self/status");
        return 1;
    }
    char line[256];
    while (fgets(line, sizeof line, f) != NULL) {
        if (strncmp(line, "Name:", 5) == 0) {
            print_field(line, "Name:");
        } else if (strncmp(line, "Pid:", 4) == 0) {
            print_field(line, "Pid:");
        } else if (strncmp(line, "PPid:", 5) == 0) {
            print_field(line, "PPid:");
        } else if (strncmp(line, "VmRSS:", 6) == 0) {
            print_field(line, "VmRSS:");
        }
    }
    fclose(f);
    return 0;
}
