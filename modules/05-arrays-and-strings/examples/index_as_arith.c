#include <stdio.h>

/* The subscript a[i] is DEFINED as *(a + i). Indexing is pointer arithmetic
   wearing friendlier clothes. */
int main(void) {
    char letters[4] = {'r', 'e', 'd', '!'};

    for (size_t i = 0; i < 4; i++) {
        printf("letters[%zu]=%c   *(letters+%zu)=%c\n",
               i, letters[i], i, *(letters + i));
    }

    /* Because a[i] == *(a+i) == *(i+a) == i[a], this is legal (if silly) C: */
    printf("2[letters] = %c  (yes, really the same as letters[2])\n", 2[letters]);
    return 0;
}
