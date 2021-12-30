
#include "algorithm/check_heap.c"
#include "algorithm/check_node.c"

TCase* heap_case(void) {
	TCase* tc = tcase_create("Heap");
	tcase_add_test(tc, test_heap_alloc);
	tcase_add_test(tc, test_heap_build_heap);
	tcase_add_test(tc, test_heap_pop);
	tcase_add_test(tc, test_heap_insert);
	tcase_add_test(tc, test_heap_decrease_dist);
	return tc;
}

TCase* node_case(void) {
	TCase* tc = tcase_create("Node");
	tcase_add_test(tc, test_node_alloc);
	tcase_add_test(tc, test_node_local_alloc);
	tcase_add_test(tc, test_node_update_time);
	return tc;
}

Suite* graph_suite(void) {
	Suite* s = suite_create("Graph");
	suite_add_tcase(s, heap_case());
	suite_add_tcase(s, node_case());
	return s;
}

int main(void) {
	int number_failed;
	Suite *s_graph;
	SRunner *sr;

	s_graph = graph_suite();
	sr = srunner_create(s_graph);

	// srunner_run_all(sr, CK_NORMAL);
	srunner_run_all(sr, CK_VERBOSE);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
