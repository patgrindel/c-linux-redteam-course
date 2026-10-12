#include <stdio.h>
#include <string.h>

/* strcmp compares strings by content: 0 if equal, negative if the first is
   "less" (earlier) than the second, positive if "greater". */
int main(void) {
    const char *a = "apple";
    const char *b = "apply";

    printf("strcmp(\"apple\", \"apple\") = %d\n", strcmp("apple", "apple"));
    printf("strcmp(\"apple\", \"apply\") = %d\n", strcmp(a, b));

    if (strcmp(a, b) == 0) {
        printf("equal\n");
    } else {
        printf("different\n");
    }

    /* NEVER use == to compare C strings: a == b compares the two ADDRESSES,
       not the text, so it is almost always the wrong answer. */
    return 0;
}
