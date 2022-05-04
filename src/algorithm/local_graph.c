
#include "algorithm/def.h"
#include "algorithm/graph.h"
#include "algorithm/graph_pi.h"
#include "algorithm/pathfind.h"
#include "network/capture.h"

char register_heartbeat(LocalNode *this, LSFD *fds, struct hop_dest *next_hops,
                        long source_addr, char **up_iface) {
  char updated = 0;
  for (int i = 0; i < this->node.n_neighbours; i++) {
    if (this->node.neighbour_ips[i] == source_addr) {
      // Reset timer
      event_timer_arm(&this->timers[i], 0, HEARTBEAT_TIMEOUT);
      // If link was DOWN then it is now UP
      if (this->node.link_statuses[i] == LINK_DOWN) {
        this->node.link_statuses[i] = LINK_UP;
#ifdef DTLSR
        ts_toggle_state(&this->ls_time_series[i], get_now());
        capture_end_iface(this->interfaces[i], next_hops);
        event_timer_arm(&fds->replay_timer, 0, REPLAY_DELAY_T);
        *up_iface = this->interfaces[i];
#endif
        updated = 1;
        log_f("%s UP", ip_to_str(source_addr));
        break;
      }
    }
  }
  return updated;
}

char hb_buffer[HB_SIZE];

char receive_heartbeat(Node *graph, LocalNode *this, LSFD *fds,
                       struct hop_dest *next_hops, char **up_iface) {
  struct sockaddr_in from;
  receive(fds->hb_sock, (void *)hb_buffer, HB_SIZE, (struct sockaddr *)&from);
  // Update node timestamp to now
  node_update_time(&this->node);
  char updated = register_heartbeat(this, fds, next_hops,
                                    (long)from.sin_addr.s_addr, up_iface);
  // if (updated) {
  //   // Update global graph
  //   update_global_this(graph, this);
  //   // send_lsa(graph, this, fds);
  // }
  return updated;
}

char timeout_heartbeat(Node *graph, LocalNode *this, int active_fd, LSFD *fds,
                       struct hop_dest *next_hops) {
  char updated = 0;
  for (int i = 0; i < this->node.n_neighbours; i++) {
    if (this->timers[i].fd == active_fd &&
        this->node.link_statuses[i] == LINK_UP) {
      this->node.link_statuses[i] = LINK_DOWN;
#ifdef DTLSR
      ts_toggle_state(&this->ls_time_series[i], get_now());
#endif
      event_timer_disarm(&this->timers[i]);
      updated = 1;
      log_f("%s DOWN", ip_to_str(this->node.neighbour_ips[i]));
      break;
    }
  }
  if (updated) {
    // Update global graph
    update_global_this(graph, this);
    // send_lsa(graph, this, fds);
  }
  return updated;
}

void local_node_update_metrics(LocalNode *this, unsigned long long now) {
  this->node.timestamp = now;
  for (int i = 0; i < this->node.n_neighbours; i++) {
#ifdef DTLSR
    // For DTLSR, we use our complex metric
    this->node.link_metrics[i] =
        ts_compute_metric(&this->ls_time_series[i], now);
#else
    // For LSR, path cost is simply the hop count
    this->node.link_metrics[i] = 1.0;
#endif
  }
}

#define ROUTER_LSA_SIZE sizeof(Node)

// Send router LSA to all neighbours except one
void send_router_lsa_except(Node *node, LSFD *fds, long source_addr) {
  for (int i = 0; i < node->n_neighbours; i++) {
    // Skip neighbour from which LSA was originally received
    if (node->neighbour_ips[i] == source_addr) continue;
    // Populate address struct
    struct sockaddr_in *neighbour_addr =
        (struct sockaddr_in *)&(routes[i].rt_dst);
    neighbour_addr->sin_port = htons(ROUTER_LSA_PORT);
    int addr_len = sizeof(*neighbour_addr);
    // Send LSA to neighbour
    sendto(fds->router_lsa_snd_sock, (const void *)node, ROUTER_LSA_SIZE,
           MSG_CONFIRM, (const struct sockaddr *)neighbour_addr, addr_len);
  }
}

// Send router LSA to all neighbours
void send_router_lsa(Node *node, LSFD *fds) {
  // 0.0.0.0 address matches no neighbours (when exact matching)
  send_router_lsa_except(node, fds, 0L);
}

char router_lsa_buffer[ROUTER_LSA_SIZE];

char receive_router_lsa(Node *graph, LocalNode *this, LSFD *fds) {
  struct sockaddr_in from;
  receive(fds->router_lsa_rec_sock, (void *)router_lsa_buffer, ROUTER_LSA_SIZE,
          (struct sockaddr *)&from);
  // log_f("LSA from %s", inet_ntoa(from.sin_addr));
  char updated = merge_in_node(graph, (Node *)router_lsa_buffer);
  if (updated) {
    send_router_lsa_except((Node *)router_lsa_buffer, fds,
                           (long)from.sin_addr.s_addr);
  }
  return updated;
}
