/* stats.c - the single definition of stats_compute (one-definition rule). */
#include "stats.h"

stats_t stats_compute(const double *values, size_t n) {
    stats_t s = { values[0], values[0], 0.0, n };
    double sum = 0.0;
    for (size_t i = 0; i < n; i++) {
        if (values[i] < s.min) {
            s.min = values[i];
        }
        if (values[i] > s.max) {
            s.max = values[i];
        }
        sum += values[i];
    }
    s.mean = sum / (double) n;
    return s;
}
