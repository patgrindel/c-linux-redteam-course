/* switch_demo.c - switch, break, and deliberate fall-through.
 *
 * Build:  gcc -std=c11 -Wall -Wextra switch_demo.c -o switch_demo
 * Run:    ./switch_demo
 *
 * switch jumps to the matching 'case' label and then runs STRAIGHT THROUGH until
 * it hits a 'break' (or the end). Forgetting 'break' is the classic switch bug:
 * control "falls through" into the next case. Sometimes that is exactly what you
 * want (stacking several labels onto one block); when it is intentional, say so.
 */
#include <stdio.h>

static const char *classify(char c)
{
    switch (c) {
        case 'a':
        case 'e':
        case 'i':
        case 'o':
        case 'u':          /* five labels, all falling into one block */
            return "vowel";
        case ' ':
        case '\t':
            return "whitespace";
        default:
            return "other";
    }
}

int main(void)
{
    const char *samples = "ab e";
    for (int i = 0; samples[i] != '\0'; i++) {
        printf("'%c' is %s\n", samples[i], classify(samples[i]));
    }

    /* A tiny calculator showing break between cases. */
    int a = 12, b = 4;
    char op = '*';
    switch (op) {
        case '+': printf("%d + %d = %d\n", a, b, a + b); break;
        case '-': printf("%d - %d = %d\n", a, b, a - b); break;
        case '*': printf("%d * %d = %d\n", a, b, a * b); break;
        case '/': printf("%d / %d = %d\n", a, b, a / b); break;
        default:  printf("unknown operator '%c'\n", op); break;
    }
    return 0;
}
