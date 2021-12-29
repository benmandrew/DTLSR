
#include <stdlib.h>
#include <check.h>
#include "algorithm/heap.h"

START_TEST(test_heap_alloc) {
	MinHeap h = minheap_alloc(64);

	ck_assert_int_eq(h.max_size, 64);
	ck_assert_int_eq(h.size, 0);

	minheap_dealloc(&h);
}
END_TEST

START_TEST(test_heap_heapify) {
	MinHeap h = minheap_alloc(4);
	for (int i = 0; i < 4; i++) {
		h.nodes[i].node_id = i;
		h.nodes[i].dist = 4 - i;
	}
	minheap_heapify(&h, 0);

	kv v = minheap_pop(&h);
	ck_assert_int_eq(v.node_id, 3);
	ck_assert_int_eq(v.dist, 0);

	v = minheap_pop(&h);
	ck_assert_int_eq(v.node_id, 2);
	ck_assert_int_eq(v.dist, 1);

	v = minheap_pop(&h);
	ck_assert_int_eq(v.node_id, 1);
	ck_assert_int_eq(v.dist, 2);

	v = minheap_pop(&h);
	ck_assert_int_eq(v.node_id, 0);
	ck_assert_int_eq(v.dist, 3);

	minheap_dealloc(&h);
}
END_TEST

START_TEST(test_heap_pop) {
	MinHeap h = minheap_alloc(16);
	
	minheap_insert(&h, 1, 5);
	ck_assert_int_eq(h.size, 1);
	kv v = minheap_pop(&h);
	ck_assert_int_eq(v.node_id, 1);
	ck_assert_int_eq(v.dist, 5);
	ck_assert_int_eq(h.size, 0);

	minheap_dealloc(&h);
}
END_TEST

START_TEST(test_heap_insert) {
	MinHeap h = minheap_alloc(4);

	minheap_insert(&h, 1, 10);
	ck_assert_int_eq(h.size, 1);
	minheap_insert(&h, 2, 0);
	ck_assert_int_eq(h.size, 2);
	minheap_insert(&h, 3, 5);
	ck_assert_int_eq(h.size, 3);

	kv v = minheap_pop(&h);
	ck_assert_int_eq(v.node_id, 2);
	ck_assert_int_eq(v.dist, 0);

	v = minheap_pop(&h);
	ck_assert_int_eq(v.node_id, 3);
	ck_assert_int_eq(v.dist, 5);

	v = minheap_pop(&h);
	ck_assert_int_eq(v.node_id, 1);
	ck_assert_int_eq(v.dist, 10);

	minheap_dealloc(&h);
}
END_TEST

START_TEST(test_heap_decrease_dist) {
	MinHeap h = minheap_alloc(16);
	minheap_insert(&h, 0, 5);
	minheap_insert(&h, 1, 10);
	minheap_insert(&h, 2, 20);
	minheap_insert(&h, 3, 15);
	minheap_decrease_dist(&h, 2, 0);

	kv v = minheap_pop(&h);
	ck_assert_int_eq(v.node_id, 2);
	ck_assert_int_eq(v.dist, 0);

	minheap_dealloc(&h);
}
END_TEST

Suite * heap_suite(void) {
	Suite *s;
	TCase *tc_core;
	s = suite_create("Heap");
	/* Core test case */
	tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_heap_alloc);
	tcase_add_test(tc_core, test_heap_heapify);
	tcase_add_test(tc_core, test_heap_pop);
	tcase_add_test(tc_core, test_heap_insert);
	tcase_add_test(tc_core, test_heap_decrease_dist);
	suite_add_tcase(s, tc_core);
	return s;
}

int main(void) {
	int number_failed;
	Suite *s;
	SRunner *sr;

	s = heap_suite();
	sr = srunner_create(s);

	// srunner_run_all(sr, CK_NORMAL);
	srunner_run_all(sr, CK_VERBOSE);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
