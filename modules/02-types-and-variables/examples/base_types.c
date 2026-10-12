/* base_types.c - the sizes and ranges of C's built-in types on this machine.
 *
 * Build:  gcc -std=c11 -Wall -Wextra base_types.c -o base_types
 * Run:    ./base_types
 *
 * Sizes are implementation-defined - the standard guarantees minimums and an
 * ordering (char <= short <= int <= long <= long long), not exact widths. That
 * is exactly why fixed-width types (next example) exist. sizeof yields a size_t,
 * printed with %zu. Ranges come from <limits.h> and <float.h>.
 */
#include <stdio.h>
#include <limits.h>
#include <float.h>

int main(void)
{
    printf("a byte on this machine is %d bits (CHAR_BIT)\n\n", CHAR_BIT);

    printf("type           size (bytes)\n");
    printf("char           %zu\n", sizeof(char));
    printf("_Bool          %zu\n", sizeof(_Bool));
    printf("short          %zu\n", sizeof(short));
    printf("int            %zu\n", sizeof(int));
    printf("long           %zu\n", sizeof(long));
    printf("long long      %zu\n", sizeof(long long));
    printf("float          %zu\n", sizeof(float));
    printf("double         %zu\n", sizeof(double));

    printf("\nsome ranges:\n");
    printf("int       : %d .. %d\n", INT_MIN, INT_MAX);
    printf("unsigned  : 0 .. %u\n", UINT_MAX);
    printf("long      : %ld .. %ld\n", LONG_MIN, LONG_MAX);
    printf("double max: %g\n", DBL_MAX);
    return 0;
}
