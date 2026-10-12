/* Exercise 3 — traffic-light state machine driven by an enum + switch.
   The final return keeps the compiler happy (every path returns a value) and
   acts as a safe default. */
#include <stdio.h>

typedef enum { LIGHT_RED, LIGHT_GREEN, LIGHT_YELLOW } Light;

Light next_light(Light l) {
    switch (l) {
        case LIGHT_RED:    return LIGHT_GREEN;
        case LIGHT_GREEN:  return LIGHT_YELLOW;
        case LIGHT_YELLOW: return LIGHT_RED;
    }
    return LIGHT_RED;   /* unreachable for valid input, but safe */
}

const char *name(Light l) {
    switch (l) {
        case LIGHT_RED:    return "RED";
        case LIGHT_GREEN:  return "GREEN";
        case LIGHT_YELLOW: return "YELLOW";
    }
    return "?";
}

int main(void) {
    Light l = LIGHT_RED;
    for (int i = 0; i < 5; i++) {
        printf("%s ", name(l));
        l = next_light(l);
    }
    putchar('\n');
    return 0;
}
