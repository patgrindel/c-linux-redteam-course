/* if_else.c - branching with if / else if / else.
 *
 * Build:  gcc -std=c11 -Wall -Wextra if_else.c -o if_else
 * Run:    ./if_else
 *
 * C has no separate boolean type in the condition: the if simply tests whether
 * an expression is NON-ZERO. 0 is false; anything else is true. A comparison
 * like (score >= 90) produces 1 or 0, which is what you usually want.
 */
#include <stdio.h>

/* Return a letter grade for a 0..100 score. */
static char grade_for(int score)
{
    if (score >= 90) {
        return 'A';
    } else if (score >= 80) {
        return 'B';
    } else if (score >= 70) {
        return 'C';
    } else {
        return 'F';
    }
}

int main(void)
{
    int scores[] = { 95, 83, 72, 40 };

    for (int i = 0; i < 4; i++) {
        int s = scores[i];
        printf("score %3d -> grade %c\n", s, grade_for(s));
    }

    /* A reminder that the condition is "non-zero", not a true/false type: */
    int flag = 5;
    if (flag) {
        printf("flag is %d, which is non-zero, so this branch runs\n", flag);
    }
    return 0;
}
