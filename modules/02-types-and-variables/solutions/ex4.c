/* ex4.c - read two integers safely and print their sum.
 *
 * Approach: a small helper reads one line with fgets (bounded, cannot overflow)
 * and parses it with strtol, validating that digits were present, nothing but
 * whitespace trails the number, and the value fits in int range. It returns 0 on
 * success and writes the parsed value through an output pointer; on any failure
 * it returns non-zero. main calls it twice and sums the results.
 *
 * (Output pointers are covered fully in Module 03/04; here they just let one
 * helper hand back both a value and a success flag.)
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

static int read_int(FILE *in, long *out)
{
    char line[64];
    if (fgets(line, sizeof line, in) == NULL) {
        return 1;
    }

    errno = 0;
    char *end;
    long value = strtol(line, &end, 10);

    if (end == line) {
        return 1;                       /* no digits at all */
    }
    if (errno == ERANGE || value < INT_MIN || value > INT_MAX) {
        return 1;                       /* out of int range */
    }
    while (*end == '\n' || *end == ' ' || *end == '\t') {
        end++;
    }
    if (*end != '\0') {
        return 1;                       /* trailing junk */
    }

    *out = value;
    return 0;
}

int main(void)
{
    long a;
    long b;

    if (read_int(stdin, &a) != 0 || read_int(stdin, &b) != 0) {
        fprintf(stderr, "invalid input: expected two integers, one per line\n");
        return 1;
    }

    printf("the sum is %ld\n", a + b);
    return 0;
}
