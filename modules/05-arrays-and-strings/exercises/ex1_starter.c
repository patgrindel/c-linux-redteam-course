#include <stdio.h>

/* TODO: return how many times c appears in s. */
size_t count_char(const char *s, char c) {
    (void)s;    /* delete these two lines once you use the parameters */
    (void)c;
    return 0;
}

int main(void) {
    printf("%zu\n", count_char("mississippi", 's'));   /* expect 4 */
    printf("%zu\n", count_char("abc", 'z'));           /* expect 0 */
    return 0;
}
