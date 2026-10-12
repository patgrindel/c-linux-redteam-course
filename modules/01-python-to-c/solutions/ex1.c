/* ex1.c - choose the exit code based on a count.
 *
 * Approach: branch on record_count. The returned EXIT_SUCCESS/EXIT_FAILURE
 * (from <stdlib.h>) is what the shell reads in $?. Change the initializer to
 * test both paths.
 */
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int record_count = 5;   /* set to 0 to exercise the failure path */

    if (record_count > 0) {
        printf("found %d records\n", record_count);
        return EXIT_SUCCESS;
    }

    printf("no records\n");
    return EXIT_FAILURE;
}
