#include <stdio.h>

/* A struct groups related values under one name. Think of it as a record,
   or a Python object with fixed fields and no methods. */
struct point {
    int x;
    int y;
};

int main(void) {
    struct point p;        /* the type is literally "struct point" */
    p.x = 3;               /* reach members with the dot operator */
    p.y = 4;
    printf("p = (%d, %d)\n", p.x, p.y);

    struct point q = {10, 20};            /* fields in declared order */
    printf("q = (%d, %d)\n", q.x, q.y);

    struct point r = {.x = 7, .y = 8};    /* designated initializers (clearer) */
    printf("r = (%d, %d)\n", r.x, r.y);
    return 0;
}
