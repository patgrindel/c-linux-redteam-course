/* Exercise 5 — function-pointer calculator.
   apply() has no idea what operation it runs; the behavior is the argument.
   This is the C version of passing a function as a value. */
#include <stdio.h>

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }

int apply(int (*op)(int, int), int x, int y) {
    return op(x, y);
}

int main(void) {
    printf("apply(add,6,4) = %d\n", apply(add, 6, 4));
    printf("apply(sub,6,4) = %d\n", apply(sub, 6, 4));
    printf("apply(mul,6,4) = %d\n", apply(mul, 6, 4));
    return 0;
}
