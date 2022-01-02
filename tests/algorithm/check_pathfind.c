
#ifndef TEST_INCLUDES
#define TEST_INCLUDES
#include <stdlib.h>
#include <check.h>
#endif

#include "algorithm/pathfind.h"

START_TEST(test_pathfind) {
	// Generate pentagonal graph
	// Graph g;
	// Node ns[5];
	// ns[0] = node_alloc(2);
	// ns[0].id = 1;
	// ns[0].neighbour_ids[0] = 2;
	// ns[0].neighbour_ids[1] = 5;
	// ns[1] = node_alloc(2);
	// ns[1].id = 2;
	// ns[1].neighbour_ids[0] = 3;
	// ns[1].neighbour_ids[1] = 1;
	// ns[2] = node_alloc(2);
	// ns[2].id = 3;
	// ns[2].neighbour_ids[0] = 4;
	// ns[2].neighbour_ids[1] = 2;
	// ns[3] = node_alloc(2);
	// ns[3].id = 4;
	// ns[3].neighbour_ids[0] = 5;
	// ns[3].neighbour_ids[1] = 3;
	// ns[4] = node_alloc(2);
	// ns[4].id = 5;
	// ns[4].neighbour_ids[0] = 1;
	// ns[4].neighbour_ids[1] = 4;
	// g.n_nodes = 5;
	// g.nodes = ns;

	// int* next_hops = pathfind(&g, 1);

	// for (int i = 0; i < MAX_NODE_NUM; i++) {
	// 	printf("id %d: %d\n", i+1, next_hops[i]);
	// }

	// free(next_hops);
	// node_dealloc(&ns[0]);
	// node_dealloc(&ns[1]);
	// node_dealloc(&ns[2]);
	// node_dealloc(&ns[3]);
	// node_dealloc(&ns[4]);

	Graph g;
	Node n;
	n = node_alloc(0);
	n.id = 1;
	g.n_nodes = 1;
	g.nodes = &n;

	int* next_hops = pathfind(&g, 1);

	for (int i = 0; i < MAX_NODE_NUM; i++) {
		printf("id %d: %d\n", i+1, next_hops[i]);
	}

	free(next_hops);
}
END_TEST





