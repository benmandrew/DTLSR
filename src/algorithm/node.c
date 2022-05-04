
#include "algorithm/node.h"

#include <string.h>

#include "algorithm/node_pi.h"

void node_init(Node *node, int n) {
  if (n > MAX_NODE_FAN) abort();
  node->state = NODE_SEEN;
  node->n_neighbours = n;
  node->timestamp = 0;
};

char nodes_eq(Node *n1, Node *n2) {
  char eq = (n1->id == n2->id && n1->state == n2->state &&
             n1->n_neighbours == n2->n_neighbours);
  if (!eq) return 0;
  for (int i = 0; i < n1->n_neighbours; i++) {
    eq &= (n1->neighbour_ids[i] == n2->neighbour_ids[i] &&
           n1->link_statuses[i] == n2->link_statuses[i] &&
           n1->link_metrics[i] == n2->link_metrics[i]);
    if (!eq) return 0;
  }
  return eq;
}

// 'eth' + max 2 digit ID + null terminator
#define IFACE_STR_MAX_LEN 6

LocalNode node_local_alloc(int id, int n, long hb_timeout) {
  LocalNode node;
  node_init(&node.node, n);
  node.node.id = id;
  node.if_arena_ptr = (char *)malloc(IFACE_STR_MAX_LEN * n * sizeof(char));
  for (int i = 0; i < n; i++) {
    node.timers[i] = event_timer_append(0, (long)hb_timeout);
    node.interfaces[i] =
        node.if_arena_ptr + i * (IFACE_STR_MAX_LEN * sizeof(char));
  }
  return node;
};

void node_local_dealloc(LocalNode *n) { free(n->if_arena_ptr); }

void node_update_time(Node *n) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  n->timestamp = (unsigned long long)(tv.tv_sec) * 1000 +
                 (unsigned long long)(tv.tv_usec) / 1000;
}
