#include <stdio.h>

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point top_left;
    Point bottom_right;
} Rect;

/* Reach nested members by chaining: through a pointer, r->corner.member. */
int area(const Rect *r) {
    int w = r->bottom_right.x - r->top_left.x;
    int h = r->bottom_right.y - r->top_left.y;
    return w * h;
}

int main(void) {
    Rect box = { {0, 0}, {4, 3} };   /* braces nest like the struct does */
    printf("top-left     = (%d, %d)\n", box.top_left.x, box.top_left.y);
    printf("bottom-right = (%d, %d)\n", box.bottom_right.x, box.bottom_right.y);
    printf("area = %d\n", area(&box));
    return 0;
}
