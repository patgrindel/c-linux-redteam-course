#include <stdio.h>

/* Adding 1 to a pointer advances it by one ELEMENT, not one byte.
   The compiler scales by sizeof(*p) for you. */
int main(void) {
    int nums[5] = {10, 20, 30, 40, 50};
    int *p = nums;   /* an array name used as a value decays to &nums[0] */

    for (size_t i = 0; i < 5; i++) {
        printf("p + %zu  ->  address %p  ->  value %d\n",
               i, (void *)(p + i), *(p + i));
    }

    printf("\nnums[2] is %d, and *(nums + 2) is %d  (same thing)\n",
           nums[2], *(nums + 2));
    printf("(p + 1) - p = %td element\n", (p + 1) - p);
    return 0;
}
