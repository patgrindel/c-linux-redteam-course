/* Exercise 1 — increment through a pointer.
   The function receives the ADDRESS of x and writes through it, so the
   caller's x actually changes. */
#include <stdio.h>

void increment(int *p) {
    *p = *p + 1;    /* equivalently: (*p)++;  note the parentheses */
}

int main(void) {
    int x = 10;
    printf("x = %d at address %p\n", x, (void *)&x);

    increment(&x);

    printf("x = %d\n", x);
    return 0;
}
