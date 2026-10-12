#include <stdio.h>
#include <string.h>

/* Copying strings is where beginners overflow buffers. The destination has a
   FIXED size; the source might be longer. These two patterns are always safe.
   We take the source at runtime (from argv) so the length is not known at
   compile time -- exactly the situation where overflows happen. */
int main(int argc, char **argv) {
    const char *src = (argc > 1) ? argv[1] : "default-source-text";
    char dst[8];    /* room for 7 characters plus the NUL */

    /* SAFE pattern #1 -- snprintf: never overflows, ALWAYS NUL-terminates. */
    snprintf(dst, sizeof dst, "%s", src);
    printf("snprintf copy: \"%s\" (len %zu)\n", dst, strlen(dst));

    /* SAFE pattern #2 -- strncpy with room for the terminator, then terminate
       by hand. strncpy does NOT add a NUL when the source is too long, so you
       MUST place one yourself. Forgetting this is the classic "missing NUL" bug. */
    char dst2[8];
    strncpy(dst2, src, sizeof dst2 - 1);
    dst2[sizeof dst2 - 1] = '\0';
    printf("strncpy copy : \"%s\" (len %zu)\n", dst2, strlen(dst2));
    return 0;
}
