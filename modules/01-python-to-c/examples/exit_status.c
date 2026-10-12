/* exit_status.c - main's return value is the program's result code.
 *
 * Build:  gcc -std=c11 -Wall -Wextra exit_status.c -o exit_status
 * Run:    ./exit_status ; echo $?
 *
 * EXIT_SUCCESS and EXIT_FAILURE (from <stdlib.h>) are portable names for the
 * conventional 0 and non-zero results. Using them documents intent.
 */
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int items_processed = 7;

    if (items_processed == 0) {
        printf("nothing to do\n");
        return EXIT_FAILURE;   /* conventionally non-zero */
    }

    printf("processed %d items\n", items_processed);
    return EXIT_SUCCESS;       /* conventionally 0 */
}
