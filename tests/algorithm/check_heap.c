
#ifndef TEST_INCLUDES
#define TEST_INCLUDES
#include <stdlib.h>
#include <check.h>
#endif

#include "algorithm/heap.h"

START_TEST(test_heap_alloc) {
	MinHeap h = minheap_alloc(64);

	ck_assert_int_eq(h.max_size, 64);
	ck_assert_int_eq(h.size, 0);

	minheap_dealloc(&h);
}
END_TEST

START_TEST(test_heap_build_heap) {
	MinHeap h = minheap_alloc(4);
	h.size = 4;
	for (int i = 0; i < 4; i++) {
		h.nodes[i].node_id = i + 1;
		h.nodes[i].dist = 4 - i;
	}
	minheap_build_heap(&h);

	kv v = minheap_pop(&h);
	ck_assert_int_eq(v.node_id, 4);
	ck_assert_int_eq(v.dist, 1);
	v = minheap_pop(&h);
	ck_assert_int_eq(v.node_id, 3);
	ck_assert_int_eq(v.dist, 2);
	v = minheap_pop(&h);
	ck_assert_int_eq(v.node_id, 2);
	ck_assert_int_eq(v.dist, 3);
	v = minheap_pop(&h);
	ck_assert_int_eq(v.node_id, 1);
	ck_assert_int_eq(v.dist, 4);

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
