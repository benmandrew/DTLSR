
#ifndef PATHFIND_H
#define PATHFIND_H

#include "algorithm/def.h"
#include "algorithm/graph.h"
#include "algorithm/heap.h"

typedef struct dijkstra_node DijkstraNode;

struct hop_dest {
  int next_hop;
  uint32_t dest_ip;
  short metric;
  // Is the link to the next hop down?
  enum LinkState next_hop_state;
};

DijkstraNode *dijkstra(Node *graph, int src_id);

void pathfind(Node *graph, int src_id, struct hop_dest *next_hops);

#endif
