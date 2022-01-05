
#ifndef TEST_INCLUDES
#define TEST_INCLUDES
#include <stdlib.h>
#include <check.h>
#endif

#include "algorithm/graph.h"
#include "algorithm/pathfind.h"

void init_unseen(Node* ns) {
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		ns[i].id = i + 1;
		ns[i].state = NODE_UNSEEN;
	}
}

/*
 * Test case where an LSA is received from a neighbour before a heartbeat,
 * thus the node is completely unseen, not just opaque as we'd usually expect
 */
START_TEST(test_ls_graph_merge_in_lsa_first) {
	Node ns1[MAX_NODE_NUM];
	Node ns2[MAX_NODE_NUM];
	init_unseen(ns1);
	init_unseen(ns2);
	ns1[0] = node_alloc(1);
	ns1[0].id = 1;
	ns1[0].neighbour_ids[0] = 2;
	ns1[0].state = NODE_SEEN;
	// ns1[0].timestamp = 100;
	ns2[1] = node_alloc(2);
	ns2[1].id = 2;
	ns2[1].neighbour_ids[0] = 1;
	ns2[1].neighbour_ids[1] = 3;
	ns2[1].state = NODE_SEEN;
	// ns2[0].timestamp = 100;
	char updated = merge_in(ns1, ns2);

	ck_assert_int_eq(updated, (char)1);
	ck_assert_int_eq(ns1[0].id, 1);
	ck_assert_int_eq(ns1[0].neighbour_ids[0], 2);
	ck_assert_int_eq(ns1[0].state, NODE_SEEN);
	ck_assert_int_eq(ns1[1].id, 2);
	ck_assert_int_eq(ns1[1].neighbour_ids[0], 1);
	ck_assert_int_eq(ns1[1].neighbour_ids[1], 3);
	ck_assert_int_eq(ns1[1].state, NODE_SEEN);

	ck_assert_int_eq(ns2[0].state, NODE_UNSEEN);
	ck_assert_int_eq(ns2[1].id, 2);
	ck_assert_int_eq(ns2[1].neighbour_ids[0], 1);
	ck_assert_int_eq(ns2[1].neighbour_ids[1], 3);
	ck_assert_int_eq(ns2[1].state, NODE_SEEN);

	node_dealloc(&ns1[0]);
	node_dealloc(&ns2[1]);
}
END_TEST

START_TEST(test_ls_graph_merge_in_lsa_after_hb) {
	Node ns1[MAX_NODE_NUM];
	Node ns2[MAX_NODE_NUM];
	init_unseen(ns1);
	init_unseen(ns2);
	// n1
	ns1[0] = node_alloc(1);
	ns1[0].id = 1;
	ns1[0].neighbour_ids[0] = 2;
	ns1[0].state = NODE_SEEN;
	// n1 neighbours
	ns1[1].state = NODE_OPAQUE;
	// n2
	ns2[1] = node_alloc(2);
	ns2[1].id = 2;
	ns2[1].neighbour_ids[0] = 1;
	ns2[1].neighbour_ids[1] = 3;
	ns2[1].state = NODE_SEEN;
	// n2 neighbours
	ns2[0].state = NODE_OPAQUE;
	ns2[2].state = NODE_OPAQUE;
	// merge
	char updated = merge_in(ns1, ns2);

	ck_assert_int_eq(updated, (char)1);
	ck_assert_int_eq(ns1[0].id, 1);
	ck_assert_int_eq(ns1[0].neighbour_ids[0], 2);
	ck_assert_int_eq(ns1[0].state, NODE_SEEN);
	ck_assert_int_eq(ns1[1].id, 2);
	ck_assert_int_eq(ns1[1].neighbour_ids[0], 1);
	ck_assert_int_eq(ns1[1].neighbour_ids[1], 3);
	ck_assert_int_eq(ns1[1].state, NODE_SEEN);
	ck_assert_int_eq(ns1[2].id, 3);
	ck_assert_int_eq(ns1[2].state, NODE_OPAQUE);

	node_dealloc(&ns1[0]);
	node_dealloc(&ns2[1]);
}
END_TEST