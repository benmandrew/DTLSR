
#include "algorithm/def.h"
#include "algorithm/graph.h"
#include "algorithm/pathfind.h"
#include "filenames.h"
#include "network/capture.h"
#include "process/route_control.h"

#ifdef DTLSR
#define PROTOCOL "dtlsr"
#define N_AUX_FDS 7
#else
#define PROTOCOL "lsr"
#define N_AUX_FDS 5
#endif

// Aggregate timer and socket file descriptors into a single array
void aggregate_fds(LocalNode *this, LSFD *fds, int n_aux_fds) {
  fds->n_event_fds = this->node.n_neighbours + n_aux_fds;
  fds->event_fds = (int *)malloc(fds->n_event_fds * sizeof(int));
  for (int i = 0; i < this->node.n_neighbours; i++) {
    fds->event_fds[i] = this->timers[i].fd;
  }
  fds->event_fds[this->node.n_neighbours] = fds->hb_sock;
  fds->event_fds[this->node.n_neighbours + 1] = fds->router_lsa_rec_sock;
  fds->event_fds[this->node.n_neighbours + 2] = fds->router_lsa_snd_timer.fd;
  fds->event_fds[this->node.n_neighbours + 3] = fds->network_lsa_rec_sock;
  fds->event_fds[this->node.n_neighbours + 4] =
      fds->network_lsa_rec_request_sock;
#ifdef DTLSR
  fds->event_fds[this->node.n_neighbours + 5] = fds->replay_timer.fd;
  fds->event_fds[this->node.n_neighbours + 6] = fds->recomputation_timer.fd;
#endif
}

LSFD init_descriptors(LocalNode *this) {
  LSFD fds;
  fds.hb_sock = get_open_socket(HB_PORT);
  fds.router_lsa_rec_sock = get_open_socket(ROUTER_LSA_PORT);
  fds.router_lsa_snd_sock = get_socket();
  fds.network_lsa_rec_sock = get_open_socket(NETWORK_LSA_PORT);
  fds.network_lsa_snd_sock = get_socket();
  fds.network_lsa_rec_request_sock = get_open_socket(NETWORK_LSA_REQUEST_PORT);
  fds.network_lsa_snd_request_sock = get_socket();
  fds.router_lsa_snd_timer = event_timer_append(0, LSA_SEND_T);
#ifdef DTLSR
  fds.replay_timer = event_timer_append(0, REPLAY_DELAY_T);
  event_timer_disarm(&fds.replay_timer);
  fds.recomputation_timer = event_timer_append(0, METRIC_RECOMPUTATION_T);
#endif
  event_append(fds.hb_sock);
  event_append(fds.router_lsa_rec_sock);
  event_append(fds.network_lsa_rec_sock);
  event_append(fds.network_lsa_rec_request_sock);
  aggregate_fds(this, &fds, N_AUX_FDS);
  return fds;
}

void close_sockets(LSFD *fds) {
  close(fds->hb_sock);
  close(fds->router_lsa_rec_sock);
  close(fds->router_lsa_snd_sock);
  close(fds->network_lsa_rec_sock);
  close(fds->network_lsa_snd_sock);
  close(fds->network_lsa_rec_request_sock);
  close(fds->network_lsa_snd_request_sock);
  event_timer_dealloc(fds->router_lsa_snd_timer);
}

void handle_event_fd(Node *graph, LocalNode *this, LSFD *fds,
                     struct hop_dest *next_hops, int active_fd,
                     char *lsa_send_status, char *graph_updated,
                     char *start_capture, char *recompute, char *update_routing,
                     char **up_iface) {
  if (active_fd == fds->hb_sock) {
    // Heartbeat receive event
    *graph_updated = receive_heartbeat(graph, this, fds, next_hops, up_iface);
  } else if (active_fd == fds->router_lsa_rec_sock) {
    // Router LSA receive event
    receive_router_lsa(graph, this, fds);
  } else if (active_fd == fds->router_lsa_snd_timer.fd) {
    // Router LSA send event
    event_timer_reset(&fds->router_lsa_snd_timer);
    *lsa_send_status = 1;
  } else if (active_fd == fds->network_lsa_rec_sock) {
    // Network LSA request receive event
    log_f("network lsa received");
    receive_network_lsa(graph, this, fds);
  } else if (active_fd == fds->network_lsa_rec_request_sock) {
    // Network LSA request receive event
    receive_network_lsa_request(graph, this, fds);
#ifdef DTLSR
  } else if (active_fd == fds->replay_timer.fd) {
    event_timer_disarm(&fds->replay_timer);
    capture_replay_iface(*up_iface, next_hops);
    capture_remove_replayed_packets(*up_iface, next_hops);
  } else if (active_fd == fds->recomputation_timer.fd) {
    event_timer_reset(&fds->recomputation_timer);
    *recompute = 1;
#endif
  } else {
    // Heartbeat timeout event
    *graph_updated = timeout_heartbeat(graph, this, active_fd, fds, next_hops);
    *start_capture = 1;
  }
  if (*graph_updated) {
    *lsa_send_status = 1;
  }
}

void start_capturing(LocalNode *this, int active_fd,
                     struct hop_dest *next_hops) {
  for (int i = 0; i < this->node.n_neighbours; i++) {
    if (this->timers[i].fd == active_fd &&
        this->node.link_statuses[i] == LINK_DOWN) {
      capture_start_iface(this->interfaces[i], next_hops);
      break;
    }
  }
}

void request_network_lsa(LSFD *fds, int n_neighbours) {
  // Send network LSA request to every neighbour
  for (int i = 0; i < n_neighbours; i++) {
    struct sockaddr_in *neighbour_addr =
        (struct sockaddr_in *)&(routes[i].rt_dst);
    neighbour_addr->sin_port = htons(NETWORK_LSA_REQUEST_PORT);
    int addr_len = sizeof(*neighbour_addr);
    // Send 'addr_len' just in case 'sendto'
    // dereferences it even with zero length
    sendto(fds->network_lsa_snd_request_sock, &addr_len, sizeof(addr_len),
           MSG_CONFIRM, (const struct sockaddr *)neighbour_addr, addr_len);
  }
}

int driver(int argc, char **argv) {
  Node graph[MAX_NODE_NUM];
  struct hop_dest next_hops[MAX_NODE_NUM];
  memset(next_hops, 0, sizeof(next_hops));
  LocalNode this;
  char *up_iface = NULL;
  graph_init(graph);
  local_node_init(&this, PROTOCOL, CONFIG, HEARTBEAT_TIMEOUT);
  update_global_this(graph, &this);
#ifdef DTLSR
  // ts_set_falloff_param(6400.0);
  // ts_set_power_param(10.0);
  capture_init(&this, next_hops);
#endif
  routes = get_routes(&this);
  LSFD fds = init_descriptors(&this);
  // If we have crashed and restarted, this requests
  // the network representation from neighbours
  request_network_lsa(&fds, this.node.n_neighbours);
  while (1) {
    int active_fd;
    char graph_updated = 0, do_send_lsa = 0, start_capture = 0, recompute = 0,
         update_routing = 0;
#ifdef DTLSR
    capture_packets();
#endif
    if ((active_fd = event_wait(fds.event_fds, fds.n_event_fds)) < 0) {
      continue;
    }
    update_routing_table(&this, next_hops);
    handle_event_fd(graph, &this, &fds, next_hops, active_fd, &do_send_lsa,
                    &graph_updated, &start_capture, &recompute, &update_routing,
                    &up_iface);
    if (do_send_lsa) {
      local_node_update_metrics(&this, get_now());
      send_router_lsa(&this.node, &this.node, &fds);
    }
    if (graph_updated || recompute) {
      local_node_update_metrics(&this, get_now());
      update_global_this(graph, &this);
      pathfind(graph, this.node.id, next_hops);
    }
    if (update_routing) {
      update_routing_table(&this, next_hops);
    }
#ifdef DTLSR
    if (start_capture) {
      start_capturing(&this, active_fd, next_hops);
    }
#endif
  }
  close_sockets(&fds);
  return 0;
}

int main(int argc, char *argv[]) {
  set_logfile_name(PROTOCOL);
  log_f(PROTOCOL " started");
  int daemonise = 0;
  int opt;
  while ((opt = getopt(argc, argv, "d")) != -1) {
    switch (opt) {
      case 'd':
        daemonise = 1;
        break;
      default:
        log_f(PROTOCOL " usage: %s [-d]", argv[0]);
        exit(EXIT_FAILURE);
    }
  }
  if (daemonise) {
    make_daemon();
    log_f("daemonisation successful");
  }
  event_init();
  return driver(argc, argv);
}
