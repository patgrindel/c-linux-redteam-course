/* ex3_buggy.c - crashes for some inputs. Find the line and bad value with gdb,
 * then fix it so it never crashes.
 *
 * Build: gcc -std=c11 -Wall -Wextra -g ex3_buggy.c -o ex3
 * Run:   ./ex3 <string> [char]      e.g. ./ex3 hello l    ./ex3 hello z
 */
#include <stdio.h>

static const char *find_first(const char *s, char c) {
    while (*s) {
        if (*s == c) {
            return s;
        }
        s++;
    }
    return NULL;                      /* not found */
}

int main(int argc, char **argv) {
    const char *s = (argc > 1) ? argv[1] : "hello";
    char c = (argc > 2) ? argv[2][0] : 'l';

    const char *p = find_first(s, c);
    printf("found '%c'\n", *p);       /* BUG: p may be NULL when c is absent */
    return 0;
}
