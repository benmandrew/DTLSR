
#ifndef HEAP_PI_H
#define HEAP_PI_H

#include <stdio.h>

#include "algorithm/heap.h"

void swap(DijkstraNode **a, DijkstraNode **b);

int parent(int i);

int left(int i);

int right(int i);

void minheap_heapify(MinHeap *h, int i);

#endif
