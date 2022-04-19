
#include "algorithm/pathfind.h"
#include "algorithm/pathfind_pi.h"

#include <math.h>

// Disgusting, but the Linux kernel uses this exact thing so oh well
#define FIELD_SIZEOF(t, f) (sizeof(((t *)0)->f))

// Copy graph nodes to the DijkstraNode struct format
DijkstraNode *dijkstra_init(Node *graph, int src_id) {
  DijkstraNode *nodes =
      (DijkstraNode *)malloc(MAX_NODE_NUM * sizeof(DijkstraNode));
  memset(nodes, 0, MAX_NODE_NUM * sizeof(DijkstraNode));
  for (int i = 0; i < MAX_NODE_NUM; i++) {
    nodes[i].id = graph[i].id;
    nodes[i].state = graph[i].state;
    // Large number `approximating' infinity; just needs to be bigger than
    // a series of max-metric links (each of which has maximum 50,000)
    nodes[i].tent_dist = 65565.0 * 65565.0;
    nodes[i].prev_id = -1;
    if (graph[i].state == NODE_SEEN) {
      if (i + 1 == src_id) {
        nodes[i].tent_dist = 0.0;
      }
      nodes[i].n_neighbours = graph[i].n_neighbours;
      memcpy(nodes[i].neighbour_ids, graph[i].neighbour_ids,
             FIELD_SIZEOF(Node, neighbour_ids));
      memcpy(nodes[i].link_statuses, graph[i].link_statuses,
             FIELD_SIZEOF(Node, link_statuses));
      memcpy(nodes[i].link_metrics, graph[i].link_metrics,
             FIELD_SIZEOF(Node, link_metrics));
    }
    // Reduce the metrics for local links to add some hysteresis to switching
    // off down links, so that other nodes will switch away first.
    // This prevents TTL failures by looping
    // if (nodes[i].id == src_id) {
    //   for (int j = 0; j < nodes[i].n_neighbours; j++) {
    //     nodes[i].link_metrics[j] = sqrt(nodes[i].link_metrics[j]);
    //   }
    // }
  }
  return nodes;
}

MinHeap heap_init(DijkstraNode *nodes) {
  MinHeap h = minheap_alloc(MAX_NODE_NUM);
  for (int i = 0; i < MAX_NODE_NUM; i++) {
    if (nodes[i].state == NODE_SEEN) {
      minheap_insert(&h, &nodes[i]);
    }
  }
  return h;
}

// Add short and double without risk of overflowing short
short s_d_safe_add(short s, double d) {
  if (d >= (double)UINT16_MAX || s >= UINT16_MAX)
    return UINT16_MAX;
  short ds = (short)d;
  short res = ds + s;
  if (res < s)
    return UINT16_MAX;
  return res;
}

int get_next_hop(Node *graph, DijkstraNode *nodes, int src_id, int dst_id,
                 short *metric, enum LinkState *next_hop_state) {
  DijkstraNode *next;
  DijkstraNode *this = &nodes[dst_id - 1];
  *metric = 1;
  // Backtrace the route, keeping track of the 'next' hop
  while (this->prev_id != -1) {
    next = this;
    this = &nodes[this->prev_id - 1];
    for (int i = 0; i < this->n_neighbours; i++) {
      if (next->id == this->neighbour_ids[i]) {
        *next_hop_state = this->link_statuses[i];
        *metric = s_d_safe_add(*metric, graph[this->id - 1].link_metrics[i]);
        break;
      }
    }
  }
  // If the source has no route to the destination
  if (this->id == dst_id) {
    return -1;
  } else {
    return next->id;
  }
}

uint32_t get_dst_ip(Node *graph, DijkstraNode *nodes, int dst_id) {
  DijkstraNode dest = nodes[dst_id - 1];
  DijkstraNode prev = nodes[dest.prev_id - 1];
  // prev is guaranteed not to be opaque so it will have
  // neighbour info stored. Previously I used dest instead
  // but it can be opaque and thus not have IP info
  for (int i = 0; i < prev.n_neighbours; i++) {
    if (dest.id == prev.neighbour_ids[i]) {
      return graph[prev.id - 1].neighbour_ips[i];
    }
  }
  abort();
}

void get_next_hops(Node *graph, DijkstraNode *nodes, int src_id,
                   struct hop_dest *next_hops) {
  for (int i = 0; i < MAX_NODE_NUM; i++) {
    if (nodes[i].state != NODE_UNSEEN && nodes[i].prev_id != -1) {
      short metric;
      enum LinkState next_hop_state;
      int next_hop =
          get_next_hop(graph, nodes, src_id, i + 1, &metric, &next_hop_state);
      next_hops[i] =
          (struct hop_dest){.next_hop = next_hop,
                            .dest_ip = get_dst_ip(graph, nodes, i + 1),
                            .metric = metric,
                            .next_hop_state = next_hop_state};
    } else {
      next_hops[i] = (struct hop_dest){.next_hop = -1,
                                       .dest_ip = 0,
                                       .metric = 0,
                                       .next_hop_state = LINK_DOWN};
    }
  }
}

DijkstraNode *dijkstra(Node *graph, int src_id) {
  DijkstraNode *nodes = dijkstra_init(graph, src_id);
  MinHeap h = heap_init(nodes);
  while (h.size > 0) {
    DijkstraNode *curr_node = minheap_pop(&h);
    if (curr_node->state == NODE_OPAQUE) {
      continue;
    }
    for (int i = 0; i < curr_node->n_neighbours; i++) {
#ifndef DTLSR
      // Discard neighbours connected to by DOWN links
      if (curr_node->link_statuses[i] == LINK_DOWN) {
        continue;
      }
#endif
      int neighbour_index = curr_node->neighbour_ids[i] - 1;
      DijkstraNode *neighbour = &nodes[neighbour_index];
      double alt = curr_node->tent_dist + curr_node->link_metrics[i];
      if (alt < neighbour->tent_dist) {
        neighbour->prev_id = curr_node->id;
        neighbour->tent_dist = alt;
        minheap_build_heap(&h);
      }
    }
  }
  minheap_dealloc(&h);
  return nodes;
}

void pathfind(Node *graph, int src_id, struct hop_dest *next_hops) {
  DijkstraNode *nodes = dijkstra(graph, src_id);
  get_next_hops(graph, nodes, src_id, next_hops);
  free(nodes);
}
