/* manual_memory.c - a first taste of "no garbage collector".
 *
 * Build:  gcc -std=c11 -Wall -Wextra manual_memory.c -o manual_memory
 * Run:    ./manual_memory
 *
 * Python frees objects for you when nothing refers to them. C does not. When you
 * ask the operating system for memory with malloc, YOU must hand it back with
 * free, exactly once. Forget, and the memory leaks until the program exits.
 *
 * Pointers and the heap get a full treatment in Modules 04 and 07 - this is only
 * a preview so the phrase "manual memory" means something concrete.
 */
#include <stdio.h>
#include <stdlib.h>   /* malloc, free */

int main(void)
{
    /* Ask for room to store 5 ints. malloc returns the address of the block,
     * or NULL if the request failed - which we must check. */
    int *numbers = malloc(5 * sizeof *numbers);
    if (numbers == NULL) {
        fprintf(stderr, "out of memory\n");
        return 1;
    }

    for (int i = 0; i < 5; i++) {
        numbers[i] = i * i;
    }

    for (int i = 0; i < 5; i++) {
        printf("numbers[%d] = %d\n", i, numbers[i]);
    }

    free(numbers);   /* give it back. Without this line, the block leaks. */
    return 0;
}
