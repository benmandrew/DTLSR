
#include <errno.h>

#include "algorithm/def.h"
#include "algorithm/graph.h"
#include "algorithm/graph_pi.h"
#include "algorithm/pathfind.h"

#define NETWORK_LSA_SIZE MAX_NODE_NUM * sizeof(Node)

void graph_init(Node *graph) {
  for (int i = 0; i < MAX_NODE_NUM; i++) {
    graph[i].id = i + 1;
    graph[i].state = NODE_UNSEEN;
    graph[i].timestamp = 0ULL;
  }
}

void update_global_this(Node *graph, LocalNode *this) {
  local_node_update_metrics(this, get_now());
  memcpy(&graph[this->node.id - 1], this, sizeof(Node));
  // Update neighbours
  for (int i = 0; i < this->node.n_neighbours; i++) {
    int id = this->node.neighbour_ids[i];
    Node *n = &graph[id - 1];
    // n->timestamp = this->node.timestamp;
    if (n->state == NODE_UNSEEN) {
      n->id = id;
      n->state = NODE_OPAQUE;
    }
  }
}

char merge_in_graph(Node *these, Node *others) {
  char updated = 0;
  for (int i = 0; i < MAX_NODE_NUM; i++) {
    Node *this = &these[i];
    Node *other = &others[i];
    // Node exists in neither graph, or only exists in our graph,
    if (other->state == NODE_UNSEEN) {
      continue;
    }
    // other knows of a node that we have never encountered
    // other state is either opaque or seen
    if (this->state == NODE_UNSEEN) {
      memcpy(this, other, sizeof(Node));
      updated = 1;
    }
    // other is a seen node that we only know of opaquely
    if (this->state == NODE_OPAQUE && other->state == NODE_SEEN) {
      memcpy(this, other, sizeof(Node));
      updated = 1;
    }
    // other is a seen node we have also seen, but a more recent version
    if (other->state == NODE_SEEN && (other->timestamp > this->timestamp)) {
      if (!nodes_eq(this, other)) {
        memcpy(this, other, sizeof(Node));
        updated = 1;
      }
    }
  }
  return updated;
}

char merge_in_node(Node *graph, Node *other) {
  Node *this = &graph[other->id - 1];
  char updated = 0;
  // Node exists in neither graph, or only exists in our graph
  if (other->state == NODE_UNSEEN) {
    return 0;
  }
  // other knows of a node that we have never encountered
  // other state is either opaque or seen
  if (this->state == NODE_UNSEEN ||
      (this->state == NODE_OPAQUE && other->state == NODE_SEEN)) {
    memcpy(this, other, sizeof(Node));
    updated = 1;
  }
  // other is a seen node we have also seen, but a more recent version
  if (other->state == NODE_SEEN && (other->timestamp > this->timestamp)) {
    if (!nodes_eq(this, other)) {
      memcpy(this, other, sizeof(Node));
      updated = 1;
    }
  }
  // if (updated) {
  // Mark neighbours of the new node as opaque if we haven't already seen them
  for (int i = 0; i < other->n_neighbours; i++) {
    int other_neighbour_index = other->neighbour_ids[i] - 1;
    if (graph[other_neighbour_index].state == NODE_UNSEEN) {
      graph[other_neighbour_index].state = NODE_OPAQUE;
    }
  }
  // }
  return updated;
}

char buffer[NETWORK_LSA_SIZE];

char receive_network_lsa(Node *graph, LocalNode *this, LSFD *fds) {
  struct sockaddr_in from;
  receive(fds->network_lsa_rec_sock, (void *)buffer, NETWORK_LSA_SIZE,
          (struct sockaddr *)&from);
  char updated = merge_in_graph(graph, (Node *)buffer);
  // if (updated) {
  //   send_network_lsa_except(graph, this, fds, (long)from.sin_addr.s_addr);
  // }
  return updated;
}

// Send LSA to a neighbour
void send_network_lsa(Node *graph, LocalNode *this, LSFD *fds, long dest_addr) {
  int i = 0;
  char found = 0;
  for (i = 0; i < this->node.n_neighbours; i++) {
    // Find neighbour from which LSA request was received
    if (this->node.neighbour_ips[i] == dest_addr) {
      found = 1;
      break;
    }
  }
  if (!found) return;
  // Populate address struct
  struct sockaddr_in *neighbour_addr =
      (struct sockaddr_in *)&(routes[i].rt_dst);
  neighbour_addr->sin_port = htons(NETWORK_LSA_PORT);
  int addr_len = sizeof(*neighbour_addr);
  // Send LSA to neighbour
  sendto(fds->network_lsa_snd_sock, (const void *)graph, NETWORK_LSA_SIZE,
         MSG_CONFIRM, (const struct sockaddr *)neighbour_addr, addr_len);
}

void receive_network_lsa_request(Node *graph, LocalNode *this, LSFD *fds) {
  char buf[sizeof(int)];
  struct sockaddr_in from;
  receive(fds->network_lsa_rec_request_sock, (void *)buf, sizeof(int),
          (struct sockaddr *)&from);
  send_network_lsa(graph, this, fds, (long)from.sin_addr.s_addr);
}

void send_network_lsa_request(Node *graph, LocalNode *this, LSFD *fds) {
  for (int i = 0; i < this->node.n_neighbours; i++) {
    // Populate address struct
    struct sockaddr_in *neighbour_addr =
        (struct sockaddr_in *)&(routes[i].rt_dst);
    neighbour_addr->sin_port = htons(NETWORK_LSA_REQUEST_PORT);
    int addr_len = sizeof(*neighbour_addr);
    // Send LSA to neighbour
    sendto(fds->network_lsa_snd_request_sock, 0, sizeof(int), MSG_CONFIRM,
           (const struct sockaddr *)neighbour_addr, addr_len);
  }
}
