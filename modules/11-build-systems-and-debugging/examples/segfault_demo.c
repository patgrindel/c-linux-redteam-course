/* segfault_demo.c - crashes on purpose; practice reading a gdb backtrace.
 *
 * Build: gcc -std=c11 -Wall -Wextra -g segfault_demo.c -o segfault_demo
 * Run:   gdb ./segfault_demo   then: run / backtrace / print
 */
#include <stddef.h>
#include <stdio.h>

static size_t count_char(const char *s, char c) {
    size_t n = 0;
    while (*s) {                 /* dereferences s; crashes if s is NULL */
        if (*s == c) {
            n++;
        }
        s++;
    }
    return n;
}

int main(void) {
    const char *name = NULL;     /* the bug: should point at a real string */
    printf("count = %zu\n", count_char(name, 'a'));
    return 0;
}
