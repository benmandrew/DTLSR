
#ifndef TEST_INCLUDES
#define TEST_INCLUDES
#include <stdlib.h>
#include <check.h>
#endif

#include "algorithm/pathfind.h"

/*
 * Generate pentagonal graph
 * (seen S)
 * 
 *    S2
 *   /  \
 * S1    S3
 *  |    |
 * S5 -- S4
 */
START_TEST(test_pathfind_pentagon) {
	Node graph[MAX_NODE_NUM];
	graph_init(graph);
	node_init(&graph[0], 2);
	graph[0].neighbour_ids[0] = 2;
	graph[0].neighbour_ids[1] = 5;
	node_init(&graph[1], 2);
	graph[1].neighbour_ids[0] = 3;
	graph[1].neighbour_ids[1] = 1;
	node_init(&graph[2], 2);
	graph[2].neighbour_ids[0] = 4;
	graph[2].neighbour_ids[1] = 2;
	node_init(&graph[3], 2);
	graph[3].neighbour_ids[0] = 5;
	graph[3].neighbour_ids[1] = 3;
	node_init(&graph[4], 2);
	graph[4].neighbour_ids[0] = 1;
	graph[4].neighbour_ids[1] = 4;
	int* next_hops = pathfind(graph, 1);

	ck_assert_int_eq(next_hops[0], -1);
	ck_assert_int_eq(next_hops[1], 2);
	ck_assert_int_eq(next_hops[2], 2);
	ck_assert_int_eq(next_hops[3], 5);
	ck_assert_int_eq(next_hops[4], 5);
	for (int i = 5; i < MAX_NODE_NUM; i++) {
		ck_assert_int_eq(next_hops[i], -1);
	}

	free(next_hops);
}
END_TEST

/*
 * Generate graph with partition
 * (seen S)
 * 
 * S1 -- S2 -- S3
 * 
 *    S4 -- S5
 */
START_TEST(test_pathfind_partition) {
	Node graph[MAX_NODE_NUM];
	graph_init(graph);
	node_init(&graph[0], 1);
	graph[0].neighbour_ids[0] = 2;
	node_init(&graph[1], 2);
	graph[1].neighbour_ids[0] = 1;
	graph[1].neighbour_ids[1] = 3;
	node_init(&graph[2], 1);
	graph[2].neighbour_ids[0] = 2;
	node_init(&graph[3], 1);
	graph[3].neighbour_ids[0] = 5;
	node_init(&graph[4], 1);
	graph[4].neighbour_ids[0] = 4;
	int* next_hops = pathfind(graph, 1);

	ck_assert_int_eq(next_hops[0], -1);
	ck_assert_int_eq(next_hops[1], 2);
	ck_assert_int_eq(next_hops[2], 2);
	ck_assert_int_eq(next_hops[3], -1);
	ck_assert_int_eq(next_hops[4], -1);
	for (int i = 5; i < MAX_NODE_NUM; i++) {
		ck_assert_int_eq(next_hops[i], -1);
	}
	
	free(next_hops);
}
END_TEST

/*
 * Generate graph with opaque nodes
 * (seen S, opaque O)
 * 
 * S1 -- S3 -- O4
 *  |          |
 * O2          S5
 */
START_TEST(test_pathfind_opaque) {
	Node graph[MAX_NODE_NUM];
	graph_init(graph);
	node_init(&graph[0], 2);
	graph[0].neighbour_ids[0] = 2;
	graph[0].neighbour_ids[1] = 3;
	graph[1].state = NODE_OPAQUE;
	node_init(&graph[2], 2);
	graph[2].neighbour_ids[0] = 1;
	graph[2].neighbour_ids[1] = 4;
	graph[3].state = NODE_OPAQUE;
	node_init(&graph[4], 1);
	graph[4].neighbour_ids[0] = 4;
	int* next_hops = pathfind(graph, 1);

	ck_assert_int_eq(next_hops[0], -1);
	ck_assert_int_eq(next_hops[1], 2);
	ck_assert_int_eq(next_hops[2], 3);
	ck_assert_int_eq(next_hops[3], 3);
	ck_assert_int_eq(next_hops[4], -1);
	for (int i = 5; i < MAX_NODE_NUM; i++) {
		ck_assert_int_eq(next_hops[i], -1);
	}
	
	free(next_hops);
}
END_TEST



