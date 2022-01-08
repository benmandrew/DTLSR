
#include "algorithm/def.h"
#include "algorithm/graph.h"
#include "filenames.h"

#define PROTOCOL "dtlsr"

LSSockets init_sockets(LocalNode *this) {
  LSSockets socks;
  socks.hb_sock = get_open_socket(HB_PORT);
  socks.lsa_rec_sock = get_open_socket(LSA_PORT);
  socks.lsa_snd_sock = get_socket();
  event_append(socks.hb_sock);
  event_append(socks.lsa_rec_sock);
  aggregate_fds(this, &socks, N_EVENT_SOCKS);
  return socks;
}

void close_sockets(LSSockets *socks) {
  close(socks->hb_sock);
  close(socks->lsa_rec_sock);
  close(socks->lsa_snd_sock);
}

int driver(int argc, char **argv) {
  Node graph[MAX_NODE_NUM];
  LocalNode this;
  graph_init(graph);
  init_this_node(&this, PROTOCOL, CONFIG, HEARTBEAT_TIMEOUT);
  update_global_this(graph, &this.node);
  routes = get_routes(&this);
  LSSockets socks = init_sockets(&this);
  while (1) {
    int active_fd;
    if ((active_fd = event_wait(socks.event_fds, socks.n_event_fds)) < 0) {
      continue;
    }
    if (active_fd == socks.hb_sock) {
      receive_heartbeat(graph, &this, &socks);
    } else if (active_fd == socks.lsa_rec_sock) {
      receive_lsa(graph, &this, &socks);
    } else {
      timeout_heartbeat(graph, &this, active_fd, &socks);
    }
  }
  close_sockets(&socks);
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
