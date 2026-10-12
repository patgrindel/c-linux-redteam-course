/* Exercise 2 — repeat a string on the heap, growing with realloc.
   We copy slen+1 bytes each time so the terminating NUL is always present;
   len tracks the length WITHOUT the NUL. On failure we free the partial
   buffer so there is no leak. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *repeat(const char *s, size_t times) {
    size_t slen = strlen(s);
    char  *out  = malloc(1);     /* start with room for just the NUL */
    if (out == NULL) {
        return NULL;
    }
    out[0] = '\0';

    size_t len = 0;
    for (size_t i = 0; i < times; i++) {
        char *tmp = realloc(out, len + slen + 1);
        if (tmp == NULL) {
            free(out);
            return NULL;
        }
        out = tmp;
        memcpy(out + len, s, slen + 1);   /* append s and its NUL */
        len += slen;
    }
    return out;
}

int main(void) {
    char *r = repeat("ab", 3);
    if (r == NULL) {
        perror("repeat");
        return 1;
    }
    printf("%s\n", r);     /* ababab */
    free(r);
    return 0;
}
