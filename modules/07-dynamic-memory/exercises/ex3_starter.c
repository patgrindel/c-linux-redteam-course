#include <stdio.h>
#include <stdlib.h>

/* Returns a heap string like "item-3". The caller owns it and must free it. */
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
        /* BUG: label is never freed -> one leak per iteration. Fix it. */
    }
    return 0;
}
