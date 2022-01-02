
#include "algorithm/def.h"
#include "algorithm/graph.h"
#include "algorithm/heap.h"

void pathfind(Graph* g) {
	Node source = this.node;
	for (int i = 0; i < g->n_nodes; i++) {

	}
}

DijkstraNode* dijkstra_init(Graph* g, int src_id) {
	DijkstraNode* nodes = (DijkstraNode*)malloc(MAX_NODE_NUM * sizeof(DijkstraNode));
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		if (g->nodes[i].id != -1) {
			nodes[i].id = g->nodes[i].id;
			if (i == src_id) {
				nodes[i].tent_dist = 0;
			} else {
				nodes[i].tent_dist = INT32_MAX;
			}
			nodes[i].neighbour_ids = g->nodes[i].neighbour_ids;
			nodes[i].n_neighbours = g->nodes[i].n_neighbours;
			nodes[i].prev_id = -1;
		}
	}
	return nodes;
}

MinHeap* heap_init(DijkstraNode* nodes) {
	MinHeap h = minheap_alloc(MAX_NODE_NUM);
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		minheap_insert(&h, &nodes[i]);
	}
	return &h;
}

int get_next_hop(DijkstraNode** node_ptrs, int src_id, int dst_id) {
	DijkstraNode* next;
	DijkstraNode* this = node_ptrs[dst_id];
	// Backtrace the route, keeping track of the 'next' hop
	while (this->prev_id != -1) {
		next = this;
		this = node_ptrs[this->prev_id];
	}
	// If the source has no route to the destination
	if (this->id == dst_id) {
		return -1;
	} else {
		return next->id;
	}
}

void dijkstra(Graph* g, int src_id) {
	DijkstraNode* nodes = dijkstra_init(g, src_id);
	MinHeap* h = heap_init(nodes);

	while (h->size > 0) {
		DijkstraNode* curr_node = minheap_pop(h);
		for (int i = 0; i < curr_node->n_neighbours; i++) {
			int alt = curr_node->tent_dist + 1;
			DijkstraNode* neighbour = &nodes[curr_node->neighbour_ids[i]];
			if (alt < neighbour->tent_dist) {
				minheap_decrease_dist(h, neighbour->id, alt);
				neighbour->prev_id = curr_node->id;
			}
		}
	}
}


