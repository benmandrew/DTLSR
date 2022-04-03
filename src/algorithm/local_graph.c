
#include "algorithm/def.h"
#include "algorithm/graph.h"
#include "algorithm/graph_pi.h"
#include "algorithm/pathfind.h"
#include "network/capture.h"

char register_heartbeat(LocalNode *this, LSFD *fds, struct hop_dest *next_hops,
                        long source_addr, char** up_iface) {
  char updated = 0;
  for (int i = 0; i < this->node.n_neighbours; i++) {
    if (this->node.neighbour_ips[i] == source_addr) {
      // Reset timer
      event_timer_arm(&this->timers[i], HEARTBEAT_TIMEOUT, 5e+8);
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

char buffer[HB_SIZE];

char receive_heartbeat(Node *graph, LocalNode *this, LSFD *fds,
                       struct hop_dest *next_hops, char** up_iface) {
  struct sockaddr_in from;
  receive(fds->hb_sock, (void *)buffer, HB_SIZE, (struct sockaddr *)&from);
  // Update node timestamp to now
  node_update_time(&this->node);
  char updated =
      register_heartbeat(this, fds, next_hops, (long)from.sin_addr.s_addr, up_iface);
  if (updated) {
    // Update global graph
    update_global_this(graph, this);
    // send_lsa(graph, this, fds);
  }
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
  for (int i = 0; i < this->node.n_neighbours; i++) {
#ifdef DTLSR
    // For DTLSR, we use our complicated metric
    this->node.link_metrics[i] =
        ts_compute_metric(&this->ls_time_series[i], now);
#else
    // For LSR, path cost is simply the hop count
    this->node.link_metrics[i] = 1.0;
#endif
  }
}
