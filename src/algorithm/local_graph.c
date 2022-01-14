
#include "algorithm/def.h"
#include "algorithm/graph.h"
#include "algorithm/graph_pi.h"
#include "algorithm/pathfind.h"

char register_heartbeat(LocalNode *this, long source_addr) {
  char updated = 0;
  for (int i = 0; i < this->node.n_neighbours; i++) {
    if (this->node.neighbour_ips[i] == source_addr) {
      // Reset timer
      event_timer_arm(&this->timers[i], HEARTBEAT_TIMEOUT, 0);
      // If link was DOWN then it is now UP
      if (this->node.link_statuses[i] == LINK_DOWN) {
        this->node.link_statuses[i] = LINK_UP;
        ts_toggle_state(&this->ls_time_series[i], get_now());
        updated = 1;
        log_f("%s UP", ip_to_str(source_addr));
        break;
      }
    }
  }
  return updated;
}

char buffer[HB_SIZE];

char receive_heartbeat(Node *graph, LocalNode *this, LSFD *socks,
                       char is_dtlsr) {
  struct sockaddr_in from;
  receive(socks->hb_sock, (void *)buffer, HB_SIZE, (struct sockaddr *)&from);
  // Update node timestamp to now
  node_update_time(&this->node);
  char updated = register_heartbeat(this, (long)from.sin_addr.s_addr);
  if (updated) {
    // Update global graph
    update_global_this(graph, this, is_dtlsr);
    send_lsa(graph, this, socks);
  }
  return updated;
}

char timeout_heartbeat(Node *graph, LocalNode *this, int active_fd,
                       LSFD *fds, char is_dtlsr) {
  char updated = 0;
  for (int i = 0; i < this->node.n_neighbours; i++) {
    if (this->timers[i].fd == active_fd) {
      if (this->node.link_statuses[i] == LINK_UP) {
        this->node.link_statuses[i] = LINK_DOWN;
        ts_toggle_state(&this->ls_time_series[i], get_now());
        event_timer_disarm(&this->timers[i]);
        updated = 1;
        log_f("%s DOWN", ip_to_str(this->node.neighbour_ips[i]));
      }
      break;
    }
  }
  if (updated) {
    // Update global graph
    update_global_this(graph, this, is_dtlsr);
    send_lsa(graph, this, fds);
  }
  return updated;
}

void local_node_update_metrics(LocalNode *this, unsigned long long now,
                               char is_dtlsr) {
  if (!is_dtlsr) {
    // For LSR, path cost is simply the hop count
    for (int i = 0; i < this->node.n_neighbours; i++) {
      this->node.link_metrics[i] = 1.0;
    }
  } else {
    // For DTLSR, we use our more complicated metric
    for (int i = 0; i < this->node.n_neighbours; i++) {
      this->node.link_metrics[i] =
          ts_compute_metric(&this->ls_time_series[i], now);
    }
  }
}
