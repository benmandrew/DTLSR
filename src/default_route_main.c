
#include <sys/ioctl.h>

#include "algorithm/graph.h"
#include "filenames.h"
#include "process/route_control.h"

#define PROTOCOL "default"

int driver(int argc, char **argv) {
  LocalNode this;
  // Creating socket file descriptor
  int ioctl_fd = get_socket();
  // Read neighbours
  local_node_init(&this, PROTOCOL, CONFIG, HEARTBEAT_TIMEOUT);
  struct rtentry *routes = get_routes(&this);
  if (this.node.n_neighbours > 1) {
    abort();
  }
  set_gateway_route(routes, this.node.neighbour_ips[0]);
  log_f("derived route");
  ioctl(ioctl_fd, SIOCADDRT, routes);
  struct in_addr a;
  a.s_addr = this.node.neighbour_ips[0];
  log_f("ioctl gateway %d", inet_ntoa(a));

  close(ioctl_fd);
  return 0;
}

int main(int argc, char *argv[]) {
  set_logfile_name("default");
  log_f("defaultroute started");
  int daemonise = 0;
  int opt;
  while ((opt = getopt(argc, argv, "d")) != -1) {
    switch (opt) {
    case 'd':
      daemonise = 1;
      break;
    default:
      log_f("defaultroute usage: %s [-d]", argv[0]);
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
