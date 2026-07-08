#ifndef STATS_H
#define STATS_H

#include <stddef.h>

typedef struct {
    size_t count;
    double sum;
    double sum_squares;
    double min;
    double max;
} StatsTracker;

void stats_tracker_reset(StatsTracker *tracker);
void stats_tracker_add(StatsTracker *tracker, double value);
double stats_tracker_mean(const StatsTracker *tracker);
double stats_tracker_rms(const StatsTracker *tracker);
double stats_tracker_stddev(const StatsTracker *tracker);

#endif