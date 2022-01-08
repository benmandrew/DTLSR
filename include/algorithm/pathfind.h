
#ifndef PATHFIND_H
#define PATHFIND_H

#include "algorithm/def.h"
#include "algorithm/graph.h"
#include "algorithm/heap.h"

struct hop_dest
{
  int next_hop;
  uint32_t dest_ip;
};

DijkstraNode *dijkstra(Node *graph, int src_id);

void pathfind(Node *graph, int src_id, struct hop_dest *next_hops);

#endif
