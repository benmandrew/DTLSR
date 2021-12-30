
#ifndef HEAP_H
#define HEAP_H

#include <stdlib.h>

typedef struct kv {
	int node_id;
	int dist;
} kv;

typedef struct minheap {
	int size;
	int max_size;
	kv* nodes;
} MinHeap;

MinHeap minheap_alloc(int max);

void minheap_dealloc(MinHeap* h);

void minheap_insert(MinHeap* h, int id, int dist);

void minheap_decrease_dist(MinHeap* h, int id, int new_dist);

void minheap_build_heap(MinHeap* h);

kv minheap_pop(MinHeap* h);

#endif
