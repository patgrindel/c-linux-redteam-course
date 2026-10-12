/* Exercise 3 — sum a range [first, last) using pointer walking only.
   This half-open-range style (begin/end pointers) is idiomatic C and mirrors
   how the standard library and C++ iterators work. */
#include <stdio.h>

int sum(const int *first, const int *last) {
    int total = 0;
    for (const int *p = first; p != last; p++) {
        total += *p;
    }
    return total;
}

int main(void) {
    int nums[5] = {10, 20, 30, 40, 50};
    printf("sum = %d\n", sum(nums, nums + 5));
    return 0;
}
