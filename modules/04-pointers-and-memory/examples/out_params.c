#include <stdio.h>
#include <stdbool.h>

/* A function can return only one value directly. The classic C idiom is to
   return a success/fail status and write the real results through pointers
   ("output parameters"). */
bool divide(int numerator, int denominator, int *quotient, int *remainder) {
    if (denominator == 0) {
        return false;           /* refuse; leave outputs untouched */
    }
    *quotient = numerator / denominator;
    *remainder = numerator % denominator;
    return true;
}

int main(void) {
    int q = 0, r = 0;

    if (divide(17, 5, &q, &r)) {
        printf("17 / 5 = %d remainder %d\n", q, r);
    }

    if (!divide(17, 0, &q, &r)) {
        printf("division by zero refused\n");
    }
    return 0;
}
