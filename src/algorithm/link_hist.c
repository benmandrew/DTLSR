
#include "algorithm/link_hist.h"
#include "algorithm/link_hist_pi.h"

#include <math.h>
#include <stdio.h>

#include "process/logging.h"

enum LinkState link_state_toggle(enum LinkState ls) {
  switch (ls) {
  case LINK_UP:
    return LINK_DOWN;
  case LINK_DOWN:
    return LINK_UP;
  }
  abort();
}

char link_state_to_bool(enum LinkState ls) {
  switch (ls) {
  case LINK_UP:
    return 1;
  case LINK_DOWN:
    return 0;
  }
  abort();
}

#ifdef DTLSR

double falloff = 320.0;
double power = 4.0;

void ts_set_falloff_param(double f) { falloff = f; }

void ts_set_power_param(double p) { power = p; }

// e^{t_r_rel/falloff} - e^{t_l_rel/falloff}
// Supposed to multiply result by falloff, but as we divide, it cancels out
double integral_between(unsigned long long t_l, unsigned long long t_r,
                        unsigned long long now) {
  double t_l_rel = (double)t_l - (double)now;
  double t_r_rel = (double)t_r - (double)now;
  return exp(t_r_rel / falloff) - exp(t_l_rel / falloff);
}

size_t last_idx(LSTimeSeries *ts) {
  if (ts->full)
    return ts->front % TS_SIZE;
  return 0;
}

double ts_compute_metric(LSTimeSeries *ts, unsigned long long now) {
  // No hysteresis for links going up
  if (ts->curr_link_state == LINK_UP) {
    return 1.0;
  }
  return 1.0 / (ts_weighted_average_uptime(ts, now) + 0.0001);
}

double ts_average_uptime(LSTimeSeries *ts, unsigned long long now) {
  if (ts->n_states == 1)
    return (double)link_state_to_bool(ts->curr_link_state);
  double summation, total;
  // Index to first element
  size_t first = (ts->front - 1) % TS_SIZE;
  // If full, we need to compute index to last element
  total = now - ts->timestamps[last_idx(ts)];
  summation = now - ts->timestamps[first];
  for (int i = 0; i > 2 - (int)ts->n_states; i -= 2) {
    int idx_left = (i + first - 2) % TS_SIZE;
    int idx_right = (i + first - 1) % TS_SIZE;
    summation += ts->timestamps[idx_right] - ts->timestamps[idx_left];
  }
  if (ts->curr_link_state == LINK_DOWN) {
    summation = total - summation;
  }
  return summation / total;
}

// Compute the average uptime weighted by an exponential falloff
double ts_weighted_average_uptime(LSTimeSeries *ts, unsigned long long now) {
  if (ts->n_states == 1)
    return (double)link_state_to_bool(ts->curr_link_state);
  double summation, total;
  // Index to first element
  size_t first = (ts->front - 1) % TS_SIZE;
  // If full, we need to compute index to last element
  total = integral_between(ts->timestamps[last_idx(ts)], now, now);
  summation = integral_between(ts->timestamps[first], now, now);
  for (int i = 0; i > 2 - (int)ts->n_states; i -= 2) {
    int idx_left = (i + first - 2) % TS_SIZE;
    int idx_right = (i + first - 1) % TS_SIZE;
    summation += integral_between(ts->timestamps[idx_left],
                                  ts->timestamps[idx_right], now);
  }
  if (ts->curr_link_state == LINK_DOWN) {
    summation = total - summation;
  }
  return pow(summation / total, power);
}

void ts_toggle_state(LSTimeSeries *ts, unsigned long long ms) {
  ts->curr_link_state = link_state_toggle(ts->curr_link_state);
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

void ts_init(LSTimeSeries *ts, enum LinkState curr_link_state,
             unsigned long long ms) {
  // This trickery is necessary for kickstarting the history
  ts->curr_link_state = link_state_toggle(curr_link_state);
  ts->front = 0;
  ts->n_states = 0;
  ts->full = 0;
  ts_toggle_state(ts, ms);
}

#endif
