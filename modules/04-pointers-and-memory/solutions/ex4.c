/* Exercise 4 — min and max through output parameters, with a status return.
   The "return bool, write results through pointers" shape is everywhere in C. */
#include <stdio.h>
#include <stdbool.h>

bool min_max(const int *a, size_t n, int *out_min, int *out_max) {
    if (n == 0) {
        return false;           /* nothing to report; leave outputs alone */
    }
    int lo = a[0];
    int hi = a[0];
    for (size_t i = 1; i < n; i++) {
        if (a[i] < lo) {
            lo = a[i];
        }
        if (a[i] > hi) {
            hi = a[i];
        }
    }
    *out_min = lo;
    *out_max = hi;
    return true;
}

int main(void) {
    int data[5] = {4, -2, 9, 0, 9};
    int lo = 0, hi = 0;

    if (min_max(data, 5, &lo, &hi)) {
        printf("min = %d, max = %d\n", lo, hi);
    }

    if (!min_max(data, 0, &lo, &hi)) {
        printf("empty: nothing to report\n");
    }
    return 0;
}
