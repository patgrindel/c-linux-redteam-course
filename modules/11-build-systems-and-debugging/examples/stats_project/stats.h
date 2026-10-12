#ifndef STATS_H
#define STATS_H

#include <stddef.h>   /* size_t */

/* Summary statistics for a set of doubles. */
typedef struct {
    double min;
    double max;
    double mean;
    size_t count;
} stats_t;

/* Compute stats over values[0..n). Requires n > 0. */
stats_t stats_compute(const double *values, size_t n);

#endif /* STATS_H */
