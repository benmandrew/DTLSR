
#include "test_inc.h"
#include "algorithm/link_hist_pi.h"

START_TEST(test_link_hist_init) {
  #ifdef DTLSR
  LSTimeSeries ts;
  ts_init(&ts, LINK_DOWN, 0);

  ck_assert_int_eq(ts.front, 1);
  ck_assert_int_eq(ts.n_states, 1);
  ck_assert_int_eq(ts.full, 0);
  #endif
}
END_TEST

START_TEST(test_link_hist_toggle_state) {
  #ifdef DTLSR
  LSTimeSeries ts;
  ts_init(&ts, LINK_DOWN, 0);

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
  #endif
}
END_TEST

START_TEST(test_link_hist_weighted_average_uptime_f32_p1) {
  #ifdef DTLSR
  double rt;
  LSTimeSeries ts;
  ts_set_falloff_param(32.0);
  ts_set_power_param(1.0);
  ts_init(&ts, LINK_DOWN, 0ULL);
  for (int i = 1; i < TS_SIZE; i++) {
    ts_toggle_state(&ts, i);
  }
  ts_toggle_state(&ts, TS_SIZE + 1);
  rt = ts_weighted_average_uptime(&ts, TS_SIZE + 3);
  ck_assert_int_eq(double_eq(rt, 0.491137), 1);

  ts_init(&ts, LINK_DOWN, 0ULL);
  ts_toggle_state(&ts, 1ULL);
  ts_toggle_state(&ts, 4ULL);
  ts_toggle_state(&ts, 5ULL);
  rt = ts_weighted_average_uptime(&ts, 8);
  ck_assert_int_eq(double_eq(rt, 0.76159386), 1);

  ts_init(&ts, LINK_UP, 0);
  ts_toggle_state(&ts, 1);
  ts_toggle_state(&ts, 4);
  ts_toggle_state(&ts, 5);
  rt = ts_weighted_average_uptime(&ts, 8);
  ck_assert_int_eq(double_eq(rt, 0.23840613), 1);

  ts_init(&ts, LINK_DOWN, 0);
  ts_toggle_state(&ts, 8);
  rt = ts_weighted_average_uptime(&ts, 20);
  ck_assert_int_eq(double_eq(rt, 0.672874), 1);

  ts_init(&ts, LINK_DOWN, 1641922462768ULL);
  ts_toggle_state(&ts, 1641922477804ULL);
  ts_toggle_state(&ts, 1641922487777ULL);
  rt = ts_weighted_average_uptime(&ts, 1641922487777ULL);
  ck_assert_int_eq(double_eq(rt, 1.0), 1);
  #endif
}
END_TEST

START_TEST(test_link_hist_weighted_average_uptime_f6400_p1) {
  #ifdef DTLSR
  double rt;
  LSTimeSeries ts;
  ts_set_falloff_param(6400.0);
  ts_set_power_param(1.0);
  ts_init(&ts, LINK_DOWN, 0ULL);
  for (int i = 1; i < TS_SIZE; i++) {
    ts_toggle_state(&ts, i);
  }
  ts_toggle_state(&ts, TS_SIZE + 1);
  rt = ts_weighted_average_uptime(&ts, TS_SIZE + 3);
  ck_assert_int_eq(double_eq(rt, 0.499959), 1);

  ts_init(&ts, LINK_DOWN, 0ULL);
  ts_toggle_state(&ts, 1ULL);
  ts_toggle_state(&ts, 4ULL);
  ts_toggle_state(&ts, 5ULL);
  rt = ts_weighted_average_uptime(&ts, 8);
  ck_assert_int_eq(double_eq(rt, 0.750059), 1);

  ts_init(&ts, LINK_UP, 0);
  ts_toggle_state(&ts, 1);
  ts_toggle_state(&ts, 4);
  ts_toggle_state(&ts, 5);
  rt = ts_weighted_average_uptime(&ts, 8);
  ck_assert_int_eq(double_eq(rt, 0.249941), 1);

  ts_init(&ts, LINK_DOWN, 0);
  ts_toggle_state(&ts, 8);
  rt = ts_weighted_average_uptime(&ts, 20);
  ck_assert_int_eq(double_eq(rt, 0.600375), 1);

  ts_init(&ts, LINK_DOWN, 1641922462768ULL);
  ts_toggle_state(&ts, 1641922477804ULL);
  ts_toggle_state(&ts, 1641922487777ULL);
  rt = ts_weighted_average_uptime(&ts, 1641922487777ULL);
  ck_assert_int_eq(double_eq(rt, 0.805687), 1);
  #endif
}
END_TEST

START_TEST(test_link_hist_weighted_average_uptime_f32_p3) {
  #ifdef DTLSR
  double rt;
  LSTimeSeries ts;
  ts_set_falloff_param(32.0);
  ts_set_power_param(3.0);
  ts_init(&ts, LINK_DOWN, 0ULL);
  for (int i = 1; i < TS_SIZE; i++) {
    ts_toggle_state(&ts, i);
  }
  ts_toggle_state(&ts, TS_SIZE + 1);
  rt = ts_weighted_average_uptime(&ts, TS_SIZE + 3);
  ck_assert_int_eq(double_eq(rt, 0.118470), 1);

  ts_init(&ts, LINK_DOWN, 0ULL);
  ts_toggle_state(&ts, 1ULL);
  ts_toggle_state(&ts, 4ULL);
  ts_toggle_state(&ts, 5ULL);
  rt = ts_weighted_average_uptime(&ts, 8);
  ck_assert_int_eq(double_eq(rt, 0.441744), 1);

  ts_init(&ts, LINK_UP, 0);
  ts_toggle_state(&ts, 1);
  ts_toggle_state(&ts, 4);
  ts_toggle_state(&ts, 5);
  rt = ts_weighted_average_uptime(&ts, 8);
  ck_assert_int_eq(double_eq(rt, 0.013550), 1);

  ts_init(&ts, LINK_DOWN, 0);
  ts_toggle_state(&ts, 8);
  rt = ts_weighted_average_uptime(&ts, 20);
  ck_assert_int_eq(double_eq(rt, 0.304651), 1);

  ts_init(&ts, LINK_DOWN, 1641922462768ULL);
  ts_toggle_state(&ts, 1641922477804ULL);
  ts_toggle_state(&ts, 1641922487777ULL);
  rt = ts_weighted_average_uptime(&ts, 1641922487777ULL);
  ck_assert_int_eq(double_eq(rt, 1.0), 1);
  #endif
}
END_TEST

START_TEST(test_link_hist_integral_between_f32) {
  #ifdef DTLSR
  unsigned long long t_l, t_r, now;
  double res;
  ts_set_falloff_param(32.0);
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
  #endif
}
END_TEST

START_TEST(test_link_hist_integral_between_f6400) {
  #ifdef DTLSR
  unsigned long long t_l, t_r, now;
  double res;
  ts_set_falloff_param(6400.0);
  t_l = 0;
  t_r = 8;
  now = 8;
  res = integral_between(t_l, t_r, now);
  ck_assert_int_eq(double_eq(res, 0.001249), 1);

  t_l = 25;
  t_r = 50;
  now = 100;
  res = integral_between(t_l, t_r, now);
  ck_assert_int_eq(double_eq(res, 0.003868), 1);
  #endif
}
END_TEST
