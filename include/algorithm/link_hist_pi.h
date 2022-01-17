
#ifndef LINK_HIST_PI_H
#define LINK_HIST_PI_H

#include "algorithm/link_hist.h"

#ifdef DTLSR

double integral_between(unsigned long long t0, unsigned long long t1,
                        unsigned long long now);

double ts_weighted_average_uptime(LSTimeSeries *ts, unsigned long long now);

#endif

#endif
