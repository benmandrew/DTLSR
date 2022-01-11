
#include "algorithm/link_hist.h"
#include "algorithm/link_hist_pi.h"

#include <math.h>
#include <stdio.h>

#define FALLOFF 32.0

// e^{t_r_rel/FALLOFF} - e^{t_l_rel/FALLOFF}
// Supposed to multiply result by FALLOFF, but as we divide, it cancels out
double integral_between(unsigned long long t_l, unsigned long long t_r,
                        unsigned long long now) {
  double t_l_rel = (double)t_l - (double)now;
  double t_r_rel = (double)t_r - (double)now;
  return exp(t_r_rel / FALLOFF) - exp(t_l_rel / FALLOFF);
}

double ts_weighted_average_uptime(LSTimeSeries *ts, unsigned long long now) {
  if (ts->n_states == 0)
    return (double)ts->curr_link_state;
  double summation, total;
  // Index to first element
  size_t first = (ts->front - 1) % TS_SIZE;
  // If full, we need to compute index to last element
  if (ts->full) {
    total = integral_between(ts->timestamps[(first + 1) % TS_SIZE], now, now);
  } else {
    total = integral_between(ts->timestamps[0], now, now);
  }
  summation = integral_between(ts->timestamps[first], now, now);
  for (int i = 0; i > 2 - (int)ts->n_states; i -= 2) {
    int idx_left = (i + first - 2) % TS_SIZE;
    int idx_right = (i + first - 1) % TS_SIZE;
    summation += integral_between(ts->timestamps[idx_left],
                                  ts->timestamps[idx_right], now);
  }
  if (!ts->curr_link_state) {
    summation = total - summation;
  }
  return summation / total;
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
