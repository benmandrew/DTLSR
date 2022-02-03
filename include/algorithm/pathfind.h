
#ifndef PATHFIND_H
#define PATHFIND_H

#include "algorithm/def.h"
#include "algorithm/graph.h"
#include "algorithm/heap.h"

typedef struct dijkstra_node DijkstraNode;

struct hop_dest {
  // 0-indexed
  int next_hop;
  uint32_t dest_ip;
  short metric;
};

DijkstraNode *dijkstra(Node *graph, int src_id);

void pathfind(Node *graph, int src_id, struct hop_dest *next_hops);

#endif
