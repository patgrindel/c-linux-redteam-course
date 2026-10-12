#include <stdio.h>

/* Writing the parameter as "int arr[]" LOOKS like an array, but C silently
   rewrites it to "int *arr". So sizeof(arr) in here is the size of a POINTER,
   which is why a function can never recover an array's length from the array
   alone -- you must pass the length as a separate argument. */
void print_info(int *arr, size_t n) {
    printf("inside function, sizeof arr = %zu (just a pointer)\n", sizeof arr);
    printf("first element via arr[0] = %d\n", arr[0]);
    printf("caller told us n = %zu elements\n", n);
}

int main(void) {
    int nums[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    size_t count = sizeof nums / sizeof nums[0];

    printf("in main, sizeof nums = %zu (the whole array), count = %zu\n",
           sizeof nums, count);
    print_info(nums, count);   /* nums "decays" to &nums[0] here */
    return 0;
}
