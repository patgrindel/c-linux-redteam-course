/* ex2_mono.c - a monolith to split into mathutil.h / mathutil.c / main.c.
 *
 * Build: gcc -std=c11 -Wall -Wextra ex2_mono.c -o mono
 * Run:   ./mono <a> <b>
 */
#include <stdio.h>
#include <stdlib.h>

/* --- these two belong in mathutil.c, declared in mathutil.h --- */
int add(int a, int b) {
    return a + b;
}

int mul(int a, int b) {
    return a * b;
}

/* --- this belongs in main.c --- */
int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <a> <b>\n", argv[0]);
        return 1;
    }
    int a = atoi(argv[1]);
    int b = atoi(argv[2]);
    printf("%d + %d = %d\n", a, b, add(a, b));
    printf("%d * %d = %d\n", a, b, mul(a, b));
    return 0;
}
