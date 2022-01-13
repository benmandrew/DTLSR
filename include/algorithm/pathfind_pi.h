
#ifndef PATHFIND_PI_H
#define PATHFIND_PI_H

#include "algorithm/pathfind.h"

#include "algorithm/def.h"
#include "algorithm/graph.h"
#include "algorithm/heap.h"

DijkstraNode *dijkstra_init(Node *graph, int src_id);

MinHeap heap_init(DijkstraNode *nodes);

int get_next_hop(Node *graph, DijkstraNode *nodes, int src_id, int dst_id, short *metric);

uint32_t get_dst_ip(Node *graph, DijkstraNode *nodes, int dst_id);

void get_next_hops(Node *graph, DijkstraNode *nodes, int src_id,
                   struct hop_dest *next_hops);

#endif
