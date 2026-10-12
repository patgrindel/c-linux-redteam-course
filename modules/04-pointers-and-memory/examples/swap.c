#include <stdio.h>

/* WRONG: parameters are COPIES. This swaps the copies and throws them away;
   the caller's variables never change. */
void swap_by_value(int a, int b) {
    int tmp = a;
    a = b;
    b = tmp;
}

/* RIGHT: pass the ADDRESSES so the function can reach back and edit the
   caller's actual variables. This is "pass by reference" in C. */
void swap(int *a, int *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int main(void) {
    int x = 1, y = 2;

    swap_by_value(x, y);
    printf("after swap_by_value: x = %d, y = %d  (unchanged!)\n", x, y);

    swap(&x, &y);
    printf("after swap         : x = %d, y = %d  (swapped)\n", x, y);
    return 0;
}
