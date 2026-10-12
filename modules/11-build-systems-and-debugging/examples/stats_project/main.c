/* main.c - argument handling and output for the stats tool.
 *
 * Build (by hand): gcc -std=c11 -Wall -Wextra -g main.c stats.c -o stats
 * Build (make):    make
 * Run:             ./stats 3 1 4 1 5 9 2 6
 */
#include <stdio.h>
#include <stdlib.h>
#include "stats.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <num> [num...]\n", argv[0]);
        return 1;
    }

    size_t n = (size_t) (argc - 1);
    double *values = malloc(n * sizeof *values);
    if (values == NULL) {
        perror("malloc");
        return 1;
    }

    for (size_t i = 0; i < n; i++) {
        values[i] = strtod(argv[i + 1], NULL);
    }

    stats_t s = stats_compute(values, n);
    printf("count=%zu min=%.3f max=%.3f mean=%.3f\n", s.count, s.min, s.max, s.mean);

    free(values);
    return 0;
}
