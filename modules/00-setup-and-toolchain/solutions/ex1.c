/* ex1.c - greet and exit 0.
 *
 * Approach: a one-line printf with a trailing newline, then "return 0" so the
 * shell sees a success exit code. Build is clean under -Wall -Wextra because we
 * include <stdio.h> (which declares printf) and use main(void).
 */
#include <stdio.h>

int main(void)
{
    printf("Hello, my name is Patrick.\n");
    return 0;
}
