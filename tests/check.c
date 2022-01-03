
#include "algorithm/check_heap.c"
#include "algorithm/check_node.c"
#include "algorithm/check_pathfind.c"

TCase* case_heap(void) {
	TCase* tc = tcase_create("Heap");
	tcase_add_test(tc, test_heap_alloc);
	tcase_add_test(tc, test_heap_build_heap);
	tcase_add_test(tc, test_heap_pop);
	tcase_add_test(tc, test_heap_insert);
	tcase_add_test(tc, test_heap_decrease_dist);
	return tc;
}

TCase* case_node(void) {
	TCase* tc = tcase_create("Node");
	tcase_add_test(tc, test_node_alloc);
	tcase_add_test(tc, test_node_local_alloc);
	tcase_add_test(tc, test_node_update_time);
	return tc;
}

TCase* case_pathfind(void) {
	TCase* tc = tcase_create("Pathfind");
	tcase_add_test(tc, test_pathfind_pentagon);
	tcase_add_test(tc, test_pathfind_partition);
	return tc;
}

Suite* suite_graph(void) {
	Suite* s = suite_create("Graph");
	suite_add_tcase(s, case_heap());
	suite_add_tcase(s, case_node());
	suite_add_tcase(s, case_pathfind());
	return s;
}

int main(void) {
	int number_failed;
	Suite *s_graph;
	SRunner *sr;

	s_graph = suite_graph();
	sr = srunner_create(s_graph);
	srunner_set_fork_status(sr, CK_NOFORK);

	// srunner_run_all(sr, CK_NORMAL);
	srunner_run_all(sr, CK_VERBOSE);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
