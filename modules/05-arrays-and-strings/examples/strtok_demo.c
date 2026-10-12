#include <stdio.h>
#include <string.h>

/* strtok splits a string on delimiter characters. It MODIFIES its input
   (writing NULs over the delimiters), so the input must be a writable array,
   never a string literal. We parse an /etc/passwd-style line. */
int main(void) {
    char line[] = "root:x:0:0:root:/root:/bin/bash";

    char *field = strtok(line, ":");   /* first call: pass the string */
    int n = 0;
    while (field != NULL) {
        printf("field %d: %s\n", n++, field);
        field = strtok(NULL, ":");     /* later calls: pass NULL to continue */
    }
    return 0;
}
