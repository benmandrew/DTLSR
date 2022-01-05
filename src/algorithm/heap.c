
#include "algorithm/heap.h"

#include <stdio.h>

void swap(DijkstraNode** a, DijkstraNode** b) {
	DijkstraNode* c = *a;
	*a = *b;
	*b = c;
}

int parent(int i) {
	return (i - 1) / 2;
}

int left(int i) {
	return 2 * i + 1;
}

int right(int i) {
	return 2 * i + 2;
}

MinHeap minheap_alloc(int max) {
	MinHeap h;
	h.size = 0;
	h.max_size = max;
	h.node_ptrs = (DijkstraNode**)malloc(max * sizeof(DijkstraNode*));
	return h;
}

void minheap_dealloc(MinHeap* h) {
	free(h->node_ptrs);
}

void minheap_insert(MinHeap* h, DijkstraNode* n) {
	int i = h->size;
	h->node_ptrs[i] = n;
	h->size++;
	while (i != 0 &&
			(*h->node_ptrs[parent(i)]).tent_dist >
			(*h->node_ptrs[i]).tent_dist) {
		swap(&h->node_ptrs[i], &h->node_ptrs[parent(i)]);
		i = parent(i);
	}
}

// 'new_dist' must be leq than existing 'dist'
void minheap_decrease_dist(MinHeap* h, int id, int new_dist) {
	int i = 0;
	char found = 0;
	for (i = 0; i < h->size; i++) {
		if ((*h->node_ptrs[i]).id == id) {
			found = 1;
			break;
		}
	}
	if (!found) return;
	(*h->node_ptrs[i]).tent_dist = new_dist;
	while (i != 0 &&
			(*h->node_ptrs[parent(i)]).tent_dist >
			(*h->node_ptrs[i]).tent_dist) {
		swap(&h->node_ptrs[i], &h->node_ptrs[parent(i)]);
		i = parent(i);
	}
}

void minheap_heapify(MinHeap* h, int i) {
    int l = left(i);
    int r = right(i);
    int smallest = i;
    if (l < h->size &&
					(*h->node_ptrs[l]).tent_dist <
					(*h->node_ptrs[i]).tent_dist)
        smallest = l;
    if (r < h->size &&
					(*h->node_ptrs[r]).tent_dist <
					(*h->node_ptrs[smallest]).tent_dist)
        smallest = r;
    if (smallest != i) {
        swap(&h->node_ptrs[i], &h->node_ptrs[smallest]);
        minheap_heapify(h, smallest);
    }
}

void minheap_build_heap(MinHeap* h) {
	for (int i = h->size / 2 - 1; i >= 0; i--) {
		minheap_heapify(h, i);
	}
}

DijkstraNode* minheap_pop(MinHeap* h) {
	if (h->size == 1) {
		h->size--;
		return h->node_ptrs[0];
	}
	DijkstraNode* root = h->node_ptrs[0];
	h->node_ptrs[0] = h->node_ptrs[h->size - 1];
	h->size--;
	minheap_heapify(h, 0);
	return root;
}

