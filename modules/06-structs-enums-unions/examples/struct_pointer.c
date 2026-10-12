#include <stdio.h>

struct point {
    int x;
    int y;
};

/* Passing a POINTER lets the function modify the caller's struct, and avoids
   copying the whole struct. Through a pointer you use -> instead of . */
void move(struct point *p, int dx, int dy) {
    p->x += dx;     /* p->x is shorthand for (*p).x */
    p->y += dy;
}

int main(void) {
    struct point p = {0, 0};
    move(&p, 3, 4);
    printf("p = (%d, %d)\n", p.x, p.y);
    return 0;
}
