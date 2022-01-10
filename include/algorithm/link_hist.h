
#ifndef LINK_HIST_H
#define LINK_HIST_H

#include <stdlib.h>

#define TS_SIZE 64

// Circular array of link state history
// Pushing to a full time series overwrites the oldest data
typedef struct link_state_time_series {
  unsigned long long timestamps[TS_SIZE];
  // Points one cell past the front element
  // If the array is full, it points at the last element
  size_t front;
  size_t n_states;
  char full;
  char curr_link_state;
} LSTimeSeries;

// Push a state change onto the history with its timestamp
void ts_toggle_state(LSTimeSeries *ts, unsigned long long ms);

// Initialise with the current link state and timestamp
void ts_init(LSTimeSeries *ts, char curr_link_state, unsigned long long ms);



#endif
