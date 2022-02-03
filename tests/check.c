
#include "algorithm/check_heap.c"
#include "algorithm/check_link_hist.c"
#include "algorithm/check_ls_graph.c"
#include "algorithm/check_node.c"
#include "algorithm/check_pathfind.c"
#include "network/check_capture.c"

#include <math.h>

#define EPSILON 0.001

char double_eq(double a, double b) { return fabs(a - b) < EPSILON; }

TCase *case_heap(void) {
  TCase *tc = tcase_create("Heap");
  tcase_add_test(tc, test_heap_alloc);
  tcase_add_test(tc, test_heap_build_heap);
  tcase_add_test(tc, test_heap_pop);
  tcase_add_test(tc, test_heap_insert);
  tcase_add_test(tc, test_heap_decrease_dist);
  return tc;
}

TCase *case_link_hist(void) {
  TCase *tc = tcase_create("Link_Hist");
  tcase_add_test(tc, test_link_hist_init);
  tcase_add_test(tc, test_link_hist_toggle_state);
  tcase_add_test(tc, test_link_hist_weighted_average_uptime_f32_p1);
  tcase_add_test(tc, test_link_hist_weighted_average_uptime_f6400_p1);
  tcase_add_test(tc, test_link_hist_weighted_average_uptime_f32_p3);
  tcase_add_test(tc, test_link_hist_integral_between_f32);
  tcase_add_test(tc, test_link_hist_integral_between_f6400);
  return tc;
}

TCase *case_ls_graph(void) {
  TCase *tc = tcase_create("LS_Graph");
  tcase_add_test(tc, test_ls_graph_merge_in_lsa_first);
  tcase_add_test(tc, test_ls_graph_merge_in_lsa_after_hb);
  return tc;
}

TCase *case_node(void) {
  TCase *tc = tcase_create("Node");
  tcase_add_test(tc, test_node_init);
  tcase_add_test(tc, test_node_local_alloc);
  tcase_add_test(tc, test_node_update_time);
  return tc;
}

TCase *case_pathfind(void) {
  TCase *tc = tcase_create("Pathfind");
  tcase_add_test(tc, test_pathfind_pentagon);
  tcase_add_test(tc, test_pathfind_partition);
  tcase_add_test(tc, test_pathfind_opaque);
  tcase_add_test(tc, test_pathfind_metric);
  return tc;
}

Suite *suite_algorithm(void) {
  Suite *s = suite_create("Algorithm");
  suite_add_tcase(s, case_heap());
  suite_add_tcase(s, case_node());
  suite_add_tcase(s, case_pathfind());
  suite_add_tcase(s, case_ls_graph());
  suite_add_tcase(s, case_link_hist());
  return s;
}

TCase *case_capture(void) {
  TCase *tc = tcase_create("Capture");
  tcase_add_test(tc, test_capture_generate_incoming_filter_exp_1);
  tcase_add_test(tc, test_capture_generate_incoming_filter_exp_6);
  tcase_add_test(tc, test_capture_generate_replay_command);
  return tc;
}

Suite *suite_network(void) {
  Suite *s = suite_create("Network");
  suite_add_tcase(s, case_capture());
  return s;
}

int main(void) {
  int number_failed;
  Suite *s_algorithm, *s_network;
  SRunner *sr;

  s_algorithm = suite_algorithm();
  s_network = suite_network();

  sr = srunner_create(s_algorithm);
  srunner_add_suite(sr, s_network);
  srunner_set_fork_status(sr, CK_NOFORK);

  // srunner_run_all(sr, CK_NORMAL);
  srunner_run_all(sr, CK_VERBOSE);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
