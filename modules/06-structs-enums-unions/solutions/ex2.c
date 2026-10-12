/* Exercise 2 — count findings on privileged ports (< 1024).
   Classic "walk an array of structs and tally a condition" loop. */
#include <stdio.h>

typedef struct {
    const char *ip;
    int         port;
} Finding;

int count_privileged(const Finding *f, size_t n) {
    int count = 0;
    for (size_t i = 0; i < n; i++) {
        if (f[i].port < 1024) {
            count++;
        }
    }
    return count;
}

int main(void) {
    Finding findings[5] = {
        {"10.0.0.5", 22},
        {"10.0.0.5", 80},
        {"10.0.0.5", 8080},
        {"10.0.0.9", 443},
        {"10.0.0.9", 3389},
    };
    size_t n = sizeof findings / sizeof findings[0];
    printf("%d privileged-port findings\n", count_privileged(findings, n));
    return 0;
}
