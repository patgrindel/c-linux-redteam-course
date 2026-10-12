#include <stdio.h>

int add(int a, int b) { return a + b; }
int sub(int a, int b) { return a - b; }
int mul(int a, int b) { return a * b; }

int main(void) {
    /* op is a POINTER TO A FUNCTION taking (int, int) and returning int. */
    int (*op)(int, int);

    op = add;                                   /* a function name decays to its address */
    printf("add(6, 4) = %d\n", op(6, 4));

    op = sub;
    printf("sub(6, 4) = %d\n", op(6, 4));

    /* A table of function pointers: a tidy way to dispatch by index. */
    int (*table[3])(int, int) = {add, sub, mul};
    const char *names[3] = {"add", "sub", "mul"};
    for (size_t i = 0; i < 3; i++) {
        printf("%s(6, 4) = %d\n", names[i], table[i](6, 4));
    }
    return 0;
}
