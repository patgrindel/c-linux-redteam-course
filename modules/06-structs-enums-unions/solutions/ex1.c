/* Exercise 1 — a Color struct passed by value. */
#include <stdio.h>

typedef struct {
    int r;
    int g;
    int b;
} Color;

int brightness(Color c) {
    return (c.r + c.g + c.b) / 3;
}

int main(void) {
    Color gray = {.r = 90, .g = 90, .b = 90};
    Color red  = {.r = 255, .g = 0, .b = 0};
    printf("%d\n", brightness(gray));
    printf("%d\n", brightness(red));
    return 0;
}
