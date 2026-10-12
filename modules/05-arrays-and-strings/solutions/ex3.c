/* Exercise 3 — reverse a string in place.
   Swap the first and last chars, move inward, stop at the middle. */
#include <stdio.h>
#include <string.h>

void reverse(char *s) {
    size_t n = strlen(s);
    for (size_t i = 0; i < n / 2; i++) {
        char tmp = s[i];
        s[i] = s[n - 1 - i];
        s[n - 1 - i] = tmp;
    }
}

int main(void) {
    char word[] = "redteam";   /* writable array, NOT a literal */
    reverse(word);
    printf("%s\n", word);       /* expect maetder */

    char one[] = "x";
    reverse(one);
    printf("%s\n", one);        /* expect x */
    return 0;
}
