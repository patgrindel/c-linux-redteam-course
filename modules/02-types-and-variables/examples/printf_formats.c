/* printf_formats.c - matching format specifiers to types.
 *
 * Build:  gcc -std=c11 -Wall -Wextra printf_formats.c -o printf_formats
 * Run:    ./printf_formats
 *
 * printf is not type-aware at runtime: it trusts your format string to say what
 * each argument is. A mismatch (e.g. %d for a double) is undefined behavior.
 * With -Wall -Wextra, gcc actually checks your format string against the
 * arguments - another reason those flags are not optional.
 */
#include <stdio.h>

int main(void)
{
    int          i   = -42;
    unsigned int u   = 42u;
    long         l   = 1234567890L;
    double       d   = 3.14159;
    char         c   = 'Z';
    const char  *s   = "hello";
    unsigned int hex = 0xDEADu;
    size_t       sz  = sizeof(int);

    printf("%%d  signed int      : %d\n", i);
    printf("%%u  unsigned int    : %u\n", u);
    printf("%%ld long            : %ld\n", l);
    printf("%%f  double          : %f\n", d);
    printf("%%.2f fixed decimals : %.2f\n", d);
    printf("%%e  scientific      : %e\n", d);
    printf("%%g  compact float   : %g\n", d);
    printf("%%c  single char     : %c\n", c);
    printf("%%s  string          : %s\n", s);
    printf("%%x  hex (lower)     : %x\n", hex);
    printf("%%o  octal           : %o\n", u);
    printf("%%zu size_t          : %zu\n", sz);
    printf("%%p  pointer         : %p\n", (void *)&i);
    printf("literal percent sign : %%\n");
    return 0;
}
