/* ex1.c - FizzBuzz.
 *
 * Approach: loop 1..30. Test the "both" case (divisible by 15) first, because a
 * multiple of both 3 and 5 would otherwise match the first single-factor branch
 * and print the wrong thing. Order matters in an if/else-if chain.
 */
#include <stdio.h>

int main(void)
{
    for (int i = 1; i <= 30; i++) {
        if (i % 15 == 0) {
            printf("FizzBuzz\n");
        } else if (i % 3 == 0) {
            printf("Fizz\n");
        } else if (i % 5 == 0) {
            printf("Buzz\n");
        } else {
            printf("%d\n", i);
        }
    }
    return 0;
}
