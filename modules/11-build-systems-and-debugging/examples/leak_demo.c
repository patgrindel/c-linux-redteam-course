/* leak_demo.c - allocates and never frees; practice reading valgrind output.
 *
 * Build: gcc -std=c11 -Wall -Wextra -g leak_demo.c -o leak_demo
 * Run:   valgrind --leak-check=full ./leak_demo
 */
#include <stdlib.h>
#include <string.h>

int main(void) {
    char *buf = malloc(64);       /* allocated... */
    if (buf == NULL) {
        return 1;
    }
    strcpy(buf, "hello");
    /* ...and never freed: a leak valgrind reports as "definitely lost".
       The fix is a single free(buf); before return. */
    return 0;
}
