
#include "algorithm/link_hist.h"

float ts_weighted_average_uptime(LSTimeSeries *ts, unsigned long long now) {
  unsigned long long total_time_ms;
  // Full, or the front ptr points to the last array element.
  if (ts->full && ts->front != TS_SIZE - 1) {
    total_time_ms = ts->timestamps[ts->front] - ts->timestamps[ts->front + 1];
  } else {
    total_time_ms = ts->timestamps[ts->front] - ts->timestamps[0];
  }

  unsigned long long sum_uptime_ms = 0;

  for (int i = ts->front; i > 0; i -= 2) {
    int idx_left = (i + ts->front) % TS_SIZE;
    int idx_right = (i + 1 + ts->front) % TS_SIZE;
    sum_uptime_ms += ts->timestamps[idx_right] - ts->timestamps[idx_left];
  }

  return sum_uptime_ms;
}

void ts_toggle_state(LSTimeSeries *ts, unsigned long long ms) {
  if (ts->front == TS_SIZE) {
    ts->full = 1;
    ts->front = 0;
  }
  ts->curr_link_state = !ts->curr_link_state;
  ts->timestamps[ts->front] = ms;
  ts->front++;
  if (!ts->full) {
    ts->n_states++;
  }
}

void ts_init(LSTimeSeries *ts, char curr_link_state) {
  ts->curr_link_state = curr_link_state;
  ts->front = 0;
  ts->n_states = 0;
  ts->full = 0;
}
