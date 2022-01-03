
#ifndef TEST_INCLUDES
#define TEST_INCLUDES
#include <stdlib.h>
#include <check.h>
#endif

#include "algorithm/pathfind.h"

START_TEST(test_pathfind_pentagon) {
	// Generate pentagonal graph
	Graph g;
	Node ns[MAX_NODE_NUM];
	ns[0] = node_alloc(2);
	ns[0].id = 1;
	ns[0].neighbour_ids[0] = 2;
	ns[0].neighbour_ids[1] = 5;
	ns[1] = node_alloc(2);
	ns[1].id = 2;
	ns[1].neighbour_ids[0] = 3;
	ns[1].neighbour_ids[1] = 1;
	ns[2] = node_alloc(2);
	ns[2].id = 3;
	ns[2].neighbour_ids[0] = 4;
	ns[2].neighbour_ids[1] = 2;
	ns[3] = node_alloc(2);
	ns[3].id = 4;
	ns[3].neighbour_ids[0] = 5;
	ns[3].neighbour_ids[1] = 3;
	ns[4] = node_alloc(2);
	ns[4].id = 5;
	ns[4].neighbour_ids[0] = 1;
	ns[4].neighbour_ids[1] = 4;
	for (int i = 5; i < MAX_NODE_NUM; i++) ns[i].id = -1;
	g.n_nodes = 5;
	g.nodes = ns;
	int* next_hops = pathfind(&g, 1);

	ck_assert_int_eq(next_hops[0], -1);
	ck_assert_int_eq(next_hops[1], 2);
	ck_assert_int_eq(next_hops[2], 2);
	ck_assert_int_eq(next_hops[3], 5);
	ck_assert_int_eq(next_hops[4], 5);
	for (int i = 5; i < MAX_NODE_NUM; i++) {
		ck_assert_int_eq(next_hops[i], -1);
	}

	node_dealloc(&ns[0]);
	node_dealloc(&ns[1]);
	node_dealloc(&ns[2]);
	node_dealloc(&ns[3]);
	node_dealloc(&ns[4]);
	free(next_hops);
}
END_TEST

START_TEST(test_pathfind_partition) {
	// Generate partitioned graph
	Graph g;
	Node ns[MAX_NODE_NUM];
	ns[0] = node_alloc(1);
	ns[0].id = 1;
	ns[0].neighbour_ids[0] = 2;
	ns[1] = node_alloc(2);
	ns[1].id = 2;
	ns[1].neighbour_ids[0] = 1;
	ns[1].neighbour_ids[1] = 3;
	ns[2] = node_alloc(1);
	ns[2].id = 3;
	ns[2].neighbour_ids[0] = 2;
	ns[3] = node_alloc(1);
	ns[3].id = 4;
	ns[3].neighbour_ids[0] = 5;
	ns[4] = node_alloc(1);
	ns[4].id = 5;
	ns[4].neighbour_ids[0] = 4;
	for (int i = 5; i < MAX_NODE_NUM; i++) ns[i].id = -1;
	g.n_nodes = 5;
	g.nodes = ns;
	int* next_hops = pathfind(&g, 1);

	ck_assert_int_eq(next_hops[0], -1);
	ck_assert_int_eq(next_hops[1], 2);
	ck_assert_int_eq(next_hops[2], 2);
	ck_assert_int_eq(next_hops[3], -1);
	ck_assert_int_eq(next_hops[4], -1);
	for (int i = 5; i < MAX_NODE_NUM; i++) {
		ck_assert_int_eq(next_hops[i], -1);
	}

	node_dealloc(&ns[0]);
	node_dealloc(&ns[1]);
	node_dealloc(&ns[2]);
	node_dealloc(&ns[3]);
	node_dealloc(&ns[4]);
	free(next_hops);
}
END_TEST



