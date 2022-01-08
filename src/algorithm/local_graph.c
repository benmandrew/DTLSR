
#include "algorithm/def.h"
#include "algorithm/graph.h"
#include "algorithm/pathfind.h"

char register_heartbeat(LocalNode *this, long source_addr) {
  char updated = 0;
  for (int i = 0; i < this->node.n_neighbours; i++) {
    if (this->node.neighbour_ips[i] == source_addr) {
      // Reset timer
      event_timer_arm(&this->timers[i], HEARTBEAT_TIMEOUT, 0);
      // If link was DOWN then it is now UP
      if (this->node.link_statuses[i] == LINK_DOWN) {
        log_f("%s UP", ip_to_str(source_addr));
        this->node.link_statuses[i] = LINK_UP;
        updated = 1;
        break;
      }
    }
  }
  return updated;
}

char buffer[HB_SIZE];

void receive_heartbeat(Node *graph, LocalNode *this, LSSockets *socks) {
  struct sockaddr_in from;
  receive(socks->hb_sock, (void *)buffer, HB_SIZE, (struct sockaddr *)&from);
  // Update node timestamp to now
  node_update_time(&this->node);
  char updated = register_heartbeat(this, (long)from.sin_addr.s_addr);
  if (updated) {
    // Update global graph
    update_global_this(graph, &this->node);
    send_lsa(graph, this, socks);
    pathfind_f(graph, this->node.id);
  }
}

void timeout_heartbeat(Node *graph, LocalNode *this, int active_fd,
                       LSSockets *socks) {
  char updated = 0;
  for (int i = 0; i < this->node.n_neighbours; i++) {
    if (this->timers[i].fd == active_fd) {
      if (this->node.link_statuses[i] == LINK_UP) {
        this->node.link_statuses[i] = LINK_DOWN;
        log_f("%s DOWN", ip_to_str(this->node.neighbour_ips[i]));
        event_timer_disarm(&this->timers[i]);
        updated = 1;
      }
      break;
    }
  }
  if (updated) {
    // Update global graph
    update_global_this(graph, &this->node);
    send_lsa(graph, this, socks);
    pathfind_f(graph, this->node.id);
  }
}

// Aggregate timer and socket file descriptors into a single array
void aggregate_fds(LocalNode *this, LSSockets *socks, int n_sockfds) {
  socks->n_event_fds = this->node.n_neighbours + n_sockfds;
  socks->event_fds = (int *)malloc(socks->n_event_fds * sizeof(int));
  int i;
  for (i = 0; i < this->node.n_neighbours; i++) {
    socks->event_fds[i] = this->timers[i].fd;
  }
  socks->event_fds[this->node.n_neighbours] = socks->hb_sock;
  socks->event_fds[this->node.n_neighbours + 1] = socks->lsa_rec_sock;
}
