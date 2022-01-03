
#include "algorithm/pathfind.h"

DijkstraNode* dijkstra_init(Graph* g, int src_id) {
	DijkstraNode* nodes = (DijkstraNode*)malloc(MAX_NODE_NUM * sizeof(DijkstraNode));
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		if (g->nodes[i].id != -1) {
			nodes[i].id = g->nodes[i].id;
			if (i + 1 == src_id) {
				nodes[i].tent_dist = 0;
			} else {
				// INT32_MAX - 100 so that incrementing doesn't cause overflow
				nodes[i].tent_dist = INT32_MAX - 100;
			}
			nodes[i].neighbour_ids = g->nodes[i].neighbour_ids;
			nodes[i].n_neighbours = g->nodes[i].n_neighbours;
			nodes[i].prev_id = -1;
		} else {
			nodes[i].id = -1;
		}
	}
	return nodes;
}

MinHeap heap_init(DijkstraNode* nodes) {
	MinHeap h = minheap_alloc(MAX_NODE_NUM);
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		if (nodes[i].id != -1) {
			minheap_insert(&h, &nodes[i]);
		}
	}
	return h;
}

int get_next_hop(DijkstraNode* nodes, int src_id, int dst_id) {
	DijkstraNode* next;
	DijkstraNode* this = &nodes[dst_id - 1];
	// Backtrace the route, keeping track of the 'next' hop
	while (this->prev_id != -1) {
		next = this;
		this = &nodes[this->prev_id - 1];
	}
	// If the source has no route to the destination
	if (this->id == dst_id) {
		return -1;
	} else {
		return next->id;
	}
}

int* get_next_hops(DijkstraNode* nodes, int src_id) {
	int* next_hop_ids = (int*)malloc(MAX_NODE_NUM * sizeof(int));
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		if (nodes[i].id != -1) {
			next_hop_ids[i] = get_next_hop(nodes, src_id, i + 1);
		} else {
			next_hop_ids[i] = -1;
		}
	}
	return next_hop_ids;
}

DijkstraNode* dijkstra(Graph* g, int src_id) {
	DijkstraNode* nodes = dijkstra_init(g, src_id);
	MinHeap h = heap_init(nodes);
	while (h.size > 0) {
		DijkstraNode* curr_node = minheap_pop(&h);
		for (int i = 0; i < curr_node->n_neighbours; i++) {
			int alt = curr_node->tent_dist + 1;
			DijkstraNode* neighbour = &nodes[curr_node->neighbour_ids[i] - 1];
			if (alt < neighbour->tent_dist) {
				// minheap_decrease_dist(&h, neighbour->id, alt);
				neighbour->prev_id = curr_node->id;
				neighbour->tent_dist = alt;
				minheap_build_heap(&h);
			}
		}
	}
	minheap_dealloc(&h);
	return nodes;
}

int* pathfind(Graph* g, int src_id) {
	DijkstraNode* nodes = dijkstra(g, src_id);
	int* next_hops = get_next_hops(nodes, src_id);
	free(nodes);
	return next_hops;
}
