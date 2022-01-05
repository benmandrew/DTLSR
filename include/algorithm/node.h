
#ifndef NODE_H
#define NODE_H

#include <stdint.h>
#include <stdlib.h>

#include "algorithm/def.h"
#include "process/fd_event.h"

enum NodeState {
  // Node has not been encountered at all
  NODE_UNSEEN,
  // Node has only been seen as a neighbour; its state is opaque
  NODE_OPAQUE,
  // Node is known from its LSA, we know its state
  NODE_SEEN
};

enum LinkState {
  LINK_DOWN,
  LINK_UP
};

/*
 * Logical representation of a network node in the graph
 */
typedef struct node {
  int id;
  // Do we know about this node from its LSA, or just opaquely as a neighbour?
  enum NodeState state;
  int n_neighbours;
  uint32_t source_ips[MAX_NODE_FAN];
  uint32_t neighbour_ips[MAX_NODE_FAN];
  int neighbour_ids[MAX_NODE_FAN];
  // Whether the link is alive or we have detected a breakage
  enum LinkState link_statuses[MAX_NODE_FAN];
  // Node last update time
  unsigned long long timestamp;
} Node;

typedef struct local_node {
  // Global node
  Node node;
  // Heartbeat timer array
  Timer* timers;
  // Outgoing interfaces
  char** interfaces;
  // Book-keeping pointer for interface string arena
  char* if_arena_ptr;
} LocalNode;

void node_init(Node* node, int n);

LocalNode node_local_alloc(int id, int n, int hb_timeout);

void node_local_dealloc(LocalNode* n);

void node_update_time(Node* n);

#endif
