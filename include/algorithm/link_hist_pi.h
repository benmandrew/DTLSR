
#ifndef LINK_HIST_PI_H
#define LINK_HIST_PI_H

#include "algorithm/link_hist.h"

struct ratio {
  unsigned long long num, den;
};

struct ratio ts_weighted_average_uptime(LSTimeSeries *ts, unsigned long long now);

#endif
