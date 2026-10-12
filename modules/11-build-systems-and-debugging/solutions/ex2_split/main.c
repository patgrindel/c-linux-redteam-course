/* main.c - uses mathutil via its header.
 *
 * Build: gcc -std=c11 -Wall -Wextra main.c mathutil.c -o split
 * Run:   ./split <a> <b>
 */
#include <stdio.h>
#include <stdlib.h>
#include "mathutil.h"

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
