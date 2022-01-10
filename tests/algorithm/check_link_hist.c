
#ifndef TEST_INCLUDES
#define TEST_INCLUDES
#include <check.h>
#include <stdlib.h>
#endif

#include <stdio.h>

#include "algorithm/link_hist_pi.h"

START_TEST(test_link_hist_init) {
  LSTimeSeries ts;
  ts_init(&ts, 0);

  ck_assert_int_eq(ts.front, 0);
  ck_assert_int_eq(ts.n_states, 0);
  ck_assert_int_eq(ts.full, 0);
}
END_TEST

START_TEST(test_link_hist_toggle_state) {
  LSTimeSeries ts;
  ts_init(&ts, 0);

  for (int i = 0; i < TS_SIZE; i++) {
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
  LSTimeSeries ts;
  ts_init(&ts, 0);
  for (int i = 0; i < TS_SIZE; i++) {
    ts_toggle_state(&ts, i);
  }

  printf("%f\n", ts_weighted_average_uptime(&ts, TS_SIZE));

  ts_init(&ts, 0);
  for (int i = 0; i < 10; i++) {
    ts_toggle_state(&ts, i);
  }

  printf("%f\n", ts_weighted_average_uptime(&ts, 10));


}
END_TEST
