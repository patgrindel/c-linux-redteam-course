/* Exercise 5 — reorder a struct's fields to minimize padding.
   "wasteful" alternates small/large/small/large, forcing padding before each
   wider member. "tight" leads with the widest field (double), then int, then
   the two chars, so later members slot in with little or no padding. */
#include <stdio.h>

struct wasteful {
    char   a;
    double b;
    char   c;
    int    d;
};

struct tight {
    double b;   /* 8-byte, place first */
    int    d;   /* 4-byte */
    char   a;   /* 1-byte */
    char   c;   /* 1-byte */
};

int main(void) {
    printf("sizeof(struct wasteful) = %zu\n", sizeof(struct wasteful));
    printf("sizeof(struct tight)    = %zu\n", sizeof(struct tight));
    return 0;
}
