
#ifndef LINK_HIST_H
#define LINK_HIST_H

#include <stdlib.h>

#define TS_SIZE 64

// Circular array of link state history
// hd_ptr points to the oldest data, tl_ptr to the newest
// Pushing to a full time series overwrites the oldest data
typedef struct link_state_time_series {
  unsigned long long timestamps[TS_SIZE];
  size_t front;
  size_t n_states;
  char full;
  char curr_link_state;
} LSTimeSeries;

float ts_weighted_average_uptime(LSTimeSeries *ts, unsigned long long now);

void ts_toggle_state(LSTimeSeries *ts, unsigned long long ms);

void ts_init(LSTimeSeries *ts, char curr_link_state);

#endif
