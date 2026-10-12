/* Exercise 3 — fix the leak.
   make_label hands ownership to the caller; the fix is a single free() inside
   the loop so each allocation is released before the next iteration. */
#include <stdio.h>
#include <stdlib.h>

char *make_label(int n) {
    char *buf = malloc(32);
    if (buf == NULL) {
        return NULL;
    }
    snprintf(buf, 32, "item-%d", n);
    return buf;
}

int main(void) {
    for (int i = 0; i < 3; i++) {
        char *label = make_label(i);
        if (label == NULL) {
            return 1;
        }
        printf("%s\n", label);
        free(label);          /* FIX: free what make_label allocated */
    }
    return 0;
}
