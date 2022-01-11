
#ifndef TEST_INCLUDES
#define TEST_INCLUDES
#include <check.h>
#include <stdlib.h>

char double_eq(double a, double b);
#endif

#include <stdio.h>

#include "algorithm/link_hist_pi.h"

START_TEST(test_link_hist_init) {
  LSTimeSeries ts;
  ts_init(&ts, 0, 0);

  ck_assert_int_eq(ts.front, 1);
  ck_assert_int_eq(ts.n_states, 1);
  ck_assert_int_eq(ts.full, 0);
}
END_TEST

START_TEST(test_link_hist_toggle_state) {
  LSTimeSeries ts;
  ts_init(&ts, 0, 0);

  for (int i = 1; i < TS_SIZE; i++) {
    ts_toggle_state(&ts, i);
  }
  for (int i = 0; i < TS_SIZE; i++) {
    ck_assert_int_eq(ts.timestamps[i], i);
  }

  // Test overwriting of previous time steps
  ts_toggle_state(&ts, TS_SIZE);
  ts_toggle_state(&ts, TS_SIZE + 1);
  ck_assert_int_eq(ts.timestamps[0], TS_SIZE);
  ck_assert_int_eq(ts.timestamps[1], TS_SIZE + 1);
}
END_TEST

START_TEST(test_link_hist_weighted_average_uptime) {
  double rt;
  LSTimeSeries ts;
  ts_init(&ts, 0, 0);
  for (int i = 1; i < TS_SIZE; i++) {
    ts_toggle_state(&ts, i);
  }
  ts_toggle_state(&ts, TS_SIZE + 1);
  rt = ts_weighted_average_uptime(&ts, TS_SIZE + 3);
  ck_assert_int_eq(double_eq(rt, 0.491137), 1);

  ts_init(&ts, 0, 0);
  ts_toggle_state(&ts, 1);
  ts_toggle_state(&ts, 4);
  ts_toggle_state(&ts, 5);
  rt = ts_weighted_average_uptime(&ts, 8);
  ck_assert_int_eq(double_eq(rt, 0.76159386), 1);

  ts_init(&ts, 1, 0);
  ts_toggle_state(&ts, 1);
  ts_toggle_state(&ts, 4);
  ts_toggle_state(&ts, 5);
  rt = ts_weighted_average_uptime(&ts, 8);
  ck_assert_int_eq(double_eq(rt, 0.23840613), 1);

  ts_init(&ts, 0, 0);
  ts_toggle_state(&ts, 8);
  rt = ts_weighted_average_uptime(&ts, 20);
  ck_assert_int_eq(double_eq(rt, 0.672874), 1);
}
END_TEST

START_TEST(test_link_hist_integral_between) {
  unsigned long long t_l, t_r, now;
  double res;
  t_l = 0;
  t_r = 8;
  now = 8;
  res = integral_between(t_l, t_r, now);
  ck_assert_int_eq(double_eq(res, 0.22119921), 1);

  t_l = 25;
  t_r = 50;
  now = 100;
  res = integral_between(t_l, t_r, now);
  ck_assert_int_eq(double_eq(res, 0.11364430), 1);
}
END_TEST
