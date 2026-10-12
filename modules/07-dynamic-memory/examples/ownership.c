#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* dup_string returns a NEW heap copy of s. The CALLER now owns it and is
   responsible for freeing it. Writing down who owns a pointer -- and therefore
   who frees it -- is half the battle in C. */
char *dup_string(const char *s) {
    size_t len = strlen(s);
    char *copy = malloc(len + 1);      /* +1 for the NUL terminator */
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, s, len + 1);          /* copy the bytes AND the NUL */
    return copy;
}

int main(void) {
    char *greeting = dup_string("hello, heap");
    if (greeting == NULL) {
        perror("dup_string");
        return 1;
    }
    printf("%s\n", greeting);

    free(greeting);     /* the caller frees what the function allocated */
    return 0;
}
