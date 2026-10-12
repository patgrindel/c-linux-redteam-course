/* ex4.c - fixed version of ex4_leaky.c.
 *
 * valgrind reported "5 blocks definitely lost ... at dup_str (malloc) / main":
 * each loop iteration malloc'd a string and dropped it. Fix: free(c) at the end
 * of each iteration. Output is unchanged; valgrind now reports 0 errors.
 *
 * Build: gcc -std=c11 -Wall -Wextra -g ex4.c -o ex4
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
        free(c);                 /* the fix: release each allocation */
    }
    return 0;
}
