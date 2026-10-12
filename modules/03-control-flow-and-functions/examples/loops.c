/* loops.c - for, while, do/while, and break/continue.
 *
 * Build:  gcc -std=c11 -Wall -Wextra loops.c -o loops
 * Run:    ./loops
 *
 * Three loop forms, one idea - repeat while a condition holds:
 *   for      : counter-driven; init / condition / step all in one place
 *   while    : condition tested BEFORE each pass (may run zero times)
 *   do/while : body runs once, THEN the condition is tested (runs >= 1 time)
 * break leaves the loop entirely; continue skips to the next iteration.
 */
#include <stdio.h>

int main(void)
{
    /* for: sum 1..5 */
    int sum = 0;
    for (int i = 1; i <= 5; i++) {
        sum += i;
    }
    printf("for: sum of 1..5 = %d\n", sum);

    /* while: count down from 3 */
    int n = 3;
    printf("while: ");
    while (n > 0) {
        printf("%d ", n);
        n--;
    }
    printf("\n");

    /* do/while: always runs at least once, even though the condition is false */
    int tries = 0;
    printf("do/while (condition false up front): ");
    do {
        printf("ran once ");
        tries++;
    } while (tries < 0);
    printf("\n");

    /* continue: skip even numbers; break: stop at 7 */
    printf("odd numbers until 7: ");
    for (int i = 1; i < 100; i++) {
        if (i == 7) {
            break;          /* leave the loop completely */
        }
        if (i % 2 == 0) {
            continue;       /* skip the rest of this iteration */
        }
        printf("%d ", i);
    }
    printf("\n");
    return 0;
}
