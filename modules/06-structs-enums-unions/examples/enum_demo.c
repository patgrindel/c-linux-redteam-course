#include <stdio.h>

/* An enum defines a set of named integer constants. By default they count up
   from 0. Use them instead of "magic numbers" for states and categories. */
typedef enum {
    SEV_INFO,       /* 0 */
    SEV_LOW,        /* 1 */
    SEV_MEDIUM,     /* 2 */
    SEV_HIGH,       /* 3 */
    SEV_CRITICAL    /* 4 */
} Severity;

const char *severity_name(Severity s) {
    switch (s) {
        case SEV_INFO:     return "INFO";
        case SEV_LOW:      return "LOW";
        case SEV_MEDIUM:   return "MEDIUM";
        case SEV_HIGH:     return "HIGH";
        case SEV_CRITICAL: return "CRITICAL";
    }
    return "UNKNOWN";   /* defensive: in case of an out-of-range value */
}

int main(void) {
    Severity finding = SEV_HIGH;
    printf("severity %d = %s\n", finding, severity_name(finding));

    for (Severity s = SEV_INFO; s <= SEV_CRITICAL; s++) {
        printf("  %d -> %s\n", s, severity_name(s));
    }
    return 0;
}
