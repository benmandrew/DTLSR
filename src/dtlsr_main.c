
#include "algorithm/def.h"
#include "algorithm/graph.h"
#include "algorithm/pathfind.h"
#include "filenames.h"
#include "process/route_control.h"
#include "network/capture.h"

#define PROTOCOL "dtlsr"
#define N_AUX_FDS 3

// Aggregate timer and socket file descriptors into a single array
void aggregate_fds(LocalNode *this, LSFD *fds, int n_aux_fds) {
  fds->n_event_fds = this->node.n_neighbours + n_aux_fds;
  fds->event_fds = (int *)malloc(fds->n_event_fds * sizeof(int));
  for (int i = 0; i < this->node.n_neighbours; i++) {
    fds->event_fds[i] = this->timers[i].fd;
  }
  fds->event_fds[this->node.n_neighbours] = fds->hb_sock;
  fds->event_fds[this->node.n_neighbours + 1] = fds->lsa_rec_sock;
  fds->event_fds[this->node.n_neighbours + 2] = fds->lsa_snd_timer.fd;
}

LSFD init_descriptors(LocalNode *this) {
  LSFD fds;
  fds.hb_sock = get_open_socket(HB_PORT);
  fds.lsa_rec_sock = get_open_socket(LSA_PORT);
  fds.lsa_snd_sock = get_socket();
  fds.lsa_snd_timer = event_timer_append(METRIC_RECOMPUTATION_T, 0);
  event_append(fds.hb_sock);
  event_append(fds.lsa_rec_sock);
  aggregate_fds(this, &fds, N_AUX_FDS);
  return fds;
}

void close_sockets(LSFD *fds) {
  close(fds->hb_sock);
  close(fds->lsa_rec_sock);
  close(fds->lsa_snd_sock);
  event_timer_dealloc(fds->lsa_snd_timer);
}

void handle_event_fd(Node *graph, LocalNode *this, LSFD *fds, struct hop_dest *next_hops,
                     int active_fd, char *send_status, char *graph_updated) {
  if (active_fd == fds->hb_sock) {
    *graph_updated = receive_heartbeat(graph, this, fds, next_hops);
  } else if (active_fd == fds->lsa_rec_sock) {
    *graph_updated = receive_lsa(graph, this, fds);
  } else if (active_fd == fds->lsa_snd_timer.fd) {
    event_timer_reset(&fds->lsa_snd_timer);
    *graph_updated = 1;
    *send_status = 1;
  } else {
    *graph_updated = timeout_heartbeat(graph, this, active_fd, fds);
  }
}

int driver(int argc, char **argv) {
  Node graph[MAX_NODE_NUM];
  struct hop_dest next_hops[MAX_NODE_NUM];
  LocalNode this;
  graph_init(graph);
  local_node_init(&this, PROTOCOL, CONFIG, HEARTBEAT_TIMEOUT);
  update_global_this(graph, &this);
  #ifdef DTLSR
  ts_set_falloff_param(64000.0);
  ts_set_power_param(10.0);
  #endif
  routes = get_routes(&this);
  LSFD fds = init_descriptors(&this);
  capture_init(&this);
  while (1) {
    int active_fd;
    char graph_updated = 0, do_send_lsa = 0;
    if (is_capturing) {
      log_f("CAP");
      capture_packets();
    }
    if ((active_fd = event_wait(fds.event_fds, fds.n_event_fds)) < 0) {
      continue;
    }
    update_routing_table(&this, next_hops);
    handle_event_fd(graph, &this, &fds, next_hops, active_fd, &do_send_lsa,
                    &graph_updated);
    if (graph_updated) {
      local_node_update_metrics(&this, get_now());
      pathfind(graph, this.node.id, next_hops);
    }
    if (do_send_lsa) {
      update_global_this(graph, &this);
      send_lsa(graph, &this, &fds);
    }
  }
  close_sockets(&fds);
  return 0;
}

int main(int argc, char *argv[]) {
  set_logfile_name("dtlsr");
  log_f("dtlsr started");
  int daemonise = 0;
  int opt;
  while ((opt = getopt(argc, argv, "d")) != -1) {
    switch (opt) {
    case 'd':
      daemonise = 1;
      break;
    default:
      log_f("dtlsr usage: %s [-d]", argv[0]);
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
