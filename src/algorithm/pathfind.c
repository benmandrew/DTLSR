
#include "algorithm/pathfind.h"
#include "algorithm/pathfind_pi.h"

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
    // INT32_MAX - 100 so that incrementing doesn't cause overflow
    nodes[i].tent_dist = INT32_MAX - 100;
    nodes[i].prev_id = -1;
    if (graph[i].state == NODE_SEEN) {
      if (i + 1 == src_id) {
        nodes[i].tent_dist = 0;
      }
      nodes[i].n_neighbours = graph[i].n_neighbours;
      memcpy(nodes[i].neighbour_ids, graph[i].neighbour_ids,
             FIELD_SIZEOF(Node, neighbour_ids));
      memcpy(nodes[i].link_statuses, graph[i].link_statuses,
             FIELD_SIZEOF(Node, link_statuses));
    }
  }
  return nodes;
}

MinHeap heap_init(DijkstraNode *nodes) {
  MinHeap h = minheap_alloc(MAX_NODE_NUM);
  for (int i = 0; i < MAX_NODE_NUM; i++) {
    if (nodes[i].state != NODE_UNSEEN) {
      minheap_insert(&h, &nodes[i]);
    }
  }
  return h;
}

// Add short and double without risk of overflowing short
short s_d_safe_add(short s, double d) {
  if (d >= (double)INT16_MAX || s >= INT16_MAX)
    return INT16_MAX;
  short ds = (short)d;
  short res = ds + s;
  if (res < s)
    return INT16_MAX;
  return res;
}

int get_next_hop(Node *graph, DijkstraNode *nodes, int src_id, int dst_id,
                 short *metric) {
  DijkstraNode *next;
  DijkstraNode *this = &nodes[dst_id - 1];
  *metric = 0;
  // Backtrace the route, keeping track of the 'next' hop
  while (this->prev_id != -1) {
    next = this;
    this = &nodes[this->prev_id - 1];
    for (int i = 0; i < this->n_neighbours; i++) {
      if (next->id == this->neighbour_ids[i]) {
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
      int next_hop = get_next_hop(graph, nodes, src_id, i + 1, &metric);
      next_hops[i] =
          (struct hop_dest){.next_hop = next_hop,
                            .dest_ip = get_dst_ip(graph, nodes, i + 1),
                            .metric = metric};
    } else {
      next_hops[i] =
          (struct hop_dest){.next_hop = -1, .dest_ip = 0, .metric = 0};
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
      // Discard neighbours connected to by DOWN links
      if (curr_node->link_statuses[i] == LINK_DOWN) {
        continue;
      }
      DijkstraNode *neighbour = &nodes[curr_node->neighbour_ids[i] - 1];
      int alt = curr_node->tent_dist + 1;
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
