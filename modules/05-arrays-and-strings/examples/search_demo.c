#include <stdio.h>
#include <string.h>

/* strchr/strrchr find a character; strstr finds a substring. Each returns a
   pointer INTO the string, or NULL if not found. Always check for NULL. */
int main(void) {
    const char *path = "/usr/local/bin/scanner";

    const char *slash = strrchr(path, '/');   /* last '/' */
    if (slash != NULL) {
        printf("basename = %s\n", slash + 1);  /* everything after the slash */
    }

    const char *hit = strstr(path, "local");
    if (hit != NULL) {
        printf("found \"local\" at offset %td\n", hit - path);
    } else {
        printf("\"local\" not found\n");
    }
    return 0;
}
