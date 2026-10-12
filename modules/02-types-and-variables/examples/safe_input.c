/* safe_input.c - reading an integer from the user the robust way.
 *
 * Build:  gcc -std=c11 -Wall -Wextra safe_input.c -o safe_input
 * Run:    echo 123 | ./safe_input
 *         echo abc | ./safe_input
 *
 * The habit to build: read a whole LINE with fgets into a fixed buffer, then
 * convert it with strtol, which (unlike scanf) tells you exactly where parsing
 * stopped and whether the value was out of range. scanf("%d", ...) is easy to
 * misuse: it leaves junk in the input stream and gives you no clean way to
 * detect trailing garbage. Prefer fgets + strtol.
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

int main(void)
{
    char line[64];

    printf("enter an integer: ");
    fflush(stdout);

    if (fgets(line, sizeof line, stdin) == NULL) {
        fprintf(stderr, "no input\n");
        return 1;
    }

    /* strtol sets 'end' to the first character it could not parse, and sets
     * errno to ERANGE on overflow. We check all of that. */
    errno = 0;
    char *end;
    long value = strtol(line, &end, 10);

    if (end == line) {
        fprintf(stderr, "that was not a number\n");
        return 1;
    }
    if (errno == ERANGE || value < INT_MIN || value > INT_MAX) {
        fprintf(stderr, "number out of range\n");
        return 1;
    }
    /* Allow a trailing newline (from Enter) or spaces, but nothing else. */
    while (*end == '\n' || *end == ' ' || *end == '\t') {
        end++;
    }
    if (*end != '\0') {
        fprintf(stderr, "trailing junk after the number: %s", end);
        return 1;
    }

    printf("you entered %ld; doubled it is %ld\n", value, value * 2);
    return 0;
}
