
#include "algorithm/heap.h"

void swap(kv* a, kv* b) {
	kv c = *a;
	*a = *b;
	*b = c;
}

inline int parent(int i) {
	return (i - 1) / 2;
}

inline int left(int i) {
	return 2 * i + 1;
}

inline int right(int i) {
	return 2 * i + 2;
}

MinHeap minheap_alloc(int max) {
	MinHeap h;
	h.size = 0;
	h.max_size = max;
	h.nodes = (kv*)malloc(max * sizeof(kv));
	return h;
}

void minheap_dealloc(MinHeap* h) {
	free(h->nodes);
}

void minheap_insert(MinHeap* h, int id, int dist) {
	int i = h->size;
	h->nodes[i] = (kv){.node_id=id, .dist=dist};
	h->size++;
	while (i != 0 && h->nodes[parent(i)].dist > h->nodes[i].dist) {
		swap(&h->nodes[i], &h->nodes[parent(i)]);
		i = parent(i);
	}
}

// 'new_dist' must be leq than existing 'dist'
void minheap_decrease_dist(MinHeap* h, int id, int new_dist) {
	int i = 0;
	char found = 0;
	for (i = 0; i < h->max_size; i++) {
		if (h->nodes[i].node_id == id) {
			found = 1;
			break;
		}
	}
	if (!found) return;
	h->nodes[i].dist = new_dist;
	while (i != 0 && h->nodes[parent(i)].dist > h->nodes[i].dist) {
		swap(&h->nodes[i], &h->nodes[parent(i)]);
		i = parent(i);
	}
}

void minheap_heapify(MinHeap* h, int i) {
    int l = left(i);
    int r = right(i);
    int smallest = i;
    if (l < h->size && h->nodes[l].dist < h->nodes[i].dist)
        smallest = l;
    if (r < h->size && h->nodes[r].dist < h->nodes[smallest].dist)
        smallest = r;
    if (smallest != i) {
        swap(&h->nodes[i], &h->nodes[smallest]);
        minheap_heapify(h, smallest);
    }
}

kv minheap_pop(MinHeap* h) {
	if (h->size == 1) {
		h->size--;
		return h->nodes[0];
	}
	kv root = h->nodes[0];
	h->nodes[0] = h->nodes[h->size - 1];
	h->size--;
	minheap_heapify(h, 0);
	return root;
}



