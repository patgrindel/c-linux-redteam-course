#include <stdio.h>

/* "const char *s" means: s points at characters we promise NOT to modify.
   We may still move the pointer (s++); we just may not write through it.
   Marking read-only inputs const documents intent and lets the compiler
   catch accidental writes. */
size_t count_spaces(const char *s) {
    size_t n = 0;
    while (*s != '\0') {
        if (*s == ' ') {
            n++;
        }
        s++;            /* allowed: we move the pointer, not the data */
        /* *s = 'x';     NOT allowed: would fail to compile */
    }
    return n;
}

int main(void) {
    const char *msg = "red team go";
    printf("\"%s\" has %zu spaces\n", msg, count_spaces(msg));
    return 0;
}
