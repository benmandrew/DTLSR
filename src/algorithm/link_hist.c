
#include "algorithm/link_hist.h"
#include "algorithm/link_hist_pi.h"

#include <stdio.h>

struct ratio ts_weighted_average_uptime(LSTimeSeries *ts,
                                        unsigned long long now) {
  if (ts->n_states == 0)
    return (struct ratio){.num = ts->curr_link_state, .den = 1};
  unsigned long long total_time_ms, sum_uptime_ms;
  // Index to first element
  size_t first = (ts->front - 1) % TS_SIZE;
  // If full, we need to compute index to last element
  if (ts->full) {
    total_time_ms = now - ts->timestamps[(first + 1) % TS_SIZE];
  } else {
    total_time_ms = now - ts->timestamps[0];
  }
  sum_uptime_ms = now - ts->timestamps[first];
  for (int i = 0; i > 2 - (int)ts->n_states; i -= 2) {
    int idx_left = (i + first - 2) % TS_SIZE;
    int idx_right = (i + first - 1) % TS_SIZE;
    sum_uptime_ms += ts->timestamps[idx_right] - ts->timestamps[idx_left];
  }
  if (!ts->curr_link_state) {
    sum_uptime_ms = total_time_ms - sum_uptime_ms;
  }
  return (struct ratio){.num = sum_uptime_ms, .den = total_time_ms};
}

void ts_toggle_state(LSTimeSeries *ts, unsigned long long ms) {
  ts->curr_link_state = !ts->curr_link_state;
  ts->timestamps[ts->front] = ms;
  ts->front++;
  if (!ts->full) {
    ts->n_states++;
  }
  if (ts->front == TS_SIZE) {
    ts->full = 1;
    ts->front = 0;
  }
}

void ts_init(LSTimeSeries *ts, char curr_link_state, unsigned long long ms) {
  // This trickery is necessary for kickstarting the history
  ts->curr_link_state = !curr_link_state;
  ts->front = 0;
  ts->n_states = 0;
  ts->full = 0;
  ts_toggle_state(ts, ms);
}
