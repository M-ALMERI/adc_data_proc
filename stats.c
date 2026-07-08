#include "stats.h"

#include <math.h>

void stats_tracker_reset(StatsTracker *tracker) {
    tracker->count = 0;
    tracker->sum = 0.0;
    tracker->sum_squares = 0.0;
    tracker->min = 0.0;
    tracker->max = 0.0;
}

void stats_tracker_add(StatsTracker *tracker, double value) {
    if (tracker->count == 0) {
        tracker->min = value;
        tracker->max = value;
    } else {
        if (value < tracker->min) {
            tracker->min = value;
        }

        if (value > tracker->max) {
            tracker->max = value;
        }
    }

    tracker->count++;
    tracker->sum += value;
    tracker->sum_squares += value * value;
}

double stats_tracker_mean(const StatsTracker *tracker) {
    if (tracker->count == 0) {
        return 0.0;
    }

    return tracker->sum / tracker->count;
}

double stats_tracker_rms(const StatsTracker *tracker) {
    if (tracker->count == 0) {
        return 0.0;
    }

    return sqrt(tracker->sum_squares / tracker->count);
}

double stats_tracker_stddev(const StatsTracker *tracker) {
    if (tracker->count == 0) {
        return 0.0;
    }

    double mean = stats_tracker_mean(tracker);
    double mean_square = tracker->sum_squares / tracker->count;
    double variance = mean_square - (mean * mean);

    if (variance < 0.0) {
        variance = 0.0;
    }

    return sqrt(variance);
}