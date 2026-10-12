/* Exercise 1 — count occurrences of a character.
   Walk to the NUL, bumping a counter on each match. */
#include <stdio.h>

size_t count_char(const char *s, char c) {
    size_t n = 0;
    for (size_t i = 0; s[i] != '\0'; i++) {
        if (s[i] == c) {
            n++;
        }
    }
    return n;
}

int main(void) {
    printf("%zu\n", count_char("mississippi", 's'));
    printf("%zu\n", count_char("abc", 'z'));
    return 0;
}
