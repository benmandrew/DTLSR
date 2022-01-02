
#ifndef HEAP_H
#define HEAP_H

#include <stdlib.h>

typedef struct dijkstra_node {
	int id;
	int tent_dist;
	int* neighbour_ids;
	int n_neighbours;
	int prev_id;
} DijkstraNode;

typedef struct minheap {
	int size;
	int max_size;
	// We simply store an array of pointers
	// into an already existing resource that
	// stores the nodes for us
	DijkstraNode** node_ptrs;
} MinHeap;

MinHeap minheap_alloc(int max);

void minheap_dealloc(MinHeap* h);

void minheap_insert(MinHeap* h, DijkstraNode* n);

void minheap_decrease_dist(MinHeap* h, int id, int new_dist);

void minheap_build_heap(MinHeap* h);

DijkstraNode* minheap_pop(MinHeap* h);

#endif
