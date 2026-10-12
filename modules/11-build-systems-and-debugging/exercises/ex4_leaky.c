/* ex4_leaky.c - leaks memory in a loop. Confirm with valgrind, then fix.
 *
 * Build: gcc -std=c11 -Wall -Wextra -g ex4_leaky.c -o ex4
 * Run:   valgrind --leak-check=full ./ex4
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *dup_str(const char *s) {
    char *d = malloc(strlen(s) + 1);
    if (d != NULL) {
        strcpy(d, s);
    }
    return d;
}

int main(void) {
    for (int i = 0; i < 5; i++) {
        char *c = dup_str("leak");
        if (c == NULL) {
            return 1;
        }
        printf("%d: %s\n", i, c);
        /* BUG: c is never freed -> 5 blocks "definitely lost" */
    }
    return 0;
}
