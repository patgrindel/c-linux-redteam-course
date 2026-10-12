#include <stdio.h>
#include <string.h>

/* strlen counts bytes up to (but not including) the NUL terminator.
   Our hand-rolled version shows exactly what it does -- and why it is O(n). */
size_t my_strlen(const char *s) {
    size_t n = 0;
    while (s[n] != '\0') {
        n++;
    }
    return n;
}

int main(void) {
    const char *s = "redteam";
    printf("strlen    = %zu\n", strlen(s));
    printf("my_strlen = %zu\n", my_strlen(s));
    printf("the array needs %zu bytes (length + 1 for the NUL)\n", strlen(s) + 1);
    return 0;
}
