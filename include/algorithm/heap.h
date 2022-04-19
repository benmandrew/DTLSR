
#ifndef HEAP_H
#define HEAP_H

#include <stdlib.h>

#include "algorithm/graph.h"

typedef struct dijkstra_node
{
	int id;
	enum NodeState state;
	double tent_dist;
	int n_neighbours;
	int neighbour_ids[MAX_NODE_FAN];
	enum LinkState link_statuses[MAX_NODE_FAN];
	double link_metrics[MAX_NODE_FAN];
	int prev_id;
} DijkstraNode;

typedef struct minheap
{
	int size;
	int max_size;
	// We simply store an array of pointers
	// into an already existing resource that
	// stores the nodes for us
	DijkstraNode **node_ptrs;
} MinHeap;

MinHeap minheap_alloc(int max);

void minheap_dealloc(MinHeap *h);

void minheap_insert(MinHeap *h, DijkstraNode *n);

void minheap_decrease_dist(MinHeap *h, int id, int new_dist);

void minheap_build_heap(MinHeap *h);

DijkstraNode *minheap_pop(MinHeap *h);

#endif
