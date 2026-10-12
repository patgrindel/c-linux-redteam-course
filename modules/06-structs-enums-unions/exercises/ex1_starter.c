#include <stdio.h>

typedef struct {
    int r;
    int g;
    int b;
} Color;

/* TODO: return the average of the three channels. */
int brightness(Color c) {
    (void)c;   /* delete once you use c */
    return 0;
}

int main(void) {
    Color gray = {.r = 90, .g = 90, .b = 90};
    Color red  = {.r = 255, .g = 0, .b = 0};
    printf("%d\n", brightness(gray));   /* expect 90 */
    printf("%d\n", brightness(red));    /* expect 85 */
    return 0;
}
