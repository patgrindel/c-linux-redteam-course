#include <stdio.h>

/* typedef gives the struct a one-word alias so you can drop the "struct"
   keyword everywhere. This anonymous-struct-with-typedef form is very common. */
typedef struct {
    int x;
    int y;
} Point;

Point add(Point a, Point b) {
    Point sum = {a.x + b.x, a.y + b.y};
    return sum;    /* small structs are returned by value (copied) -- fine */
}

int main(void) {
    Point a = {1, 2};
    Point b = {3, 4};
    Point c = add(a, b);
    printf("c = (%d, %d)\n", c.x, c.y);
    return 0;
}
