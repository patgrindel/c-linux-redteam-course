/* ex3.c - fixed version of ex3_buggy.c.
 *
 * gdb pointed at the printf line (ex3_buggy.c:24): p was 0x0 (NULL) because the
 * searched character was absent, and dereferencing NULL with *p segfaulted.
 * Fix: check p for NULL and report "not found" instead of dereferencing it.
 *
 * Build: gcc -std=c11 -Wall -Wextra -g ex3.c -o ex3
 * Run:   ./ex3 hello l    ./ex3 hello z
 */
#include <stdio.h>

static const char *find_first(const char *s, char c) {
    while (*s) {
        if (*s == c) {
            return s;
        }
        s++;
    }
    return NULL;
}

int main(int argc, char **argv) {
    const char *s = (argc > 1) ? argv[1] : "hello";
    char c = (argc > 2) ? argv[2][0] : 'l';

    const char *p = find_first(s, c);
    if (p == NULL) {
        printf("'%c' not found\n", c);
        return 1;
    }
    printf("found '%c'\n", *p);
    return 0;
}
