/* Exercise 2 — three-way rotate via output parameters.
   Save one value in a temp, then shift. Classic pointer juggling. */
#include <stdio.h>

void rotate3(int *a, int *b, int *c) {
    int tmp = *a;   /* stash old a */
    *a = *b;
    *b = *c;
    *c = tmp;
}

int main(void) {
    int a = 1, b = 2, c = 3;
    printf("before: %d %d %d\n", a, b, c);

    rotate3(&a, &b, &c);

    printf("after : %d %d %d\n", a, b, c);
    return 0;
}
