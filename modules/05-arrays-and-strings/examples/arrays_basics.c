#include <stdio.h>

/* An array is a fixed-size, contiguous block of elements of one type.
   Unlike a Python list it cannot grow, and it does not carry its own length. */
int main(void) {
    int scores[5] = {90, 80, 70, 60, 50};

    /* The one place you can recover an array's length: where it is declared,
       using sizeof. This trick does NOT work through a function (see array_decay). */
    size_t count = sizeof scores / sizeof scores[0];

    printf("the array holds %zu ints\n", count);
    printf("sizeof scores    = %zu bytes\n", sizeof scores);
    printf("sizeof scores[0] = %zu bytes\n", sizeof scores[0]);

    for (size_t i = 0; i < count; i++) {
        printf("scores[%zu] = %d\n", i, scores[i]);
    }
    return 0;
}
