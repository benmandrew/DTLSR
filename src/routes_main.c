
#include <errno.h>

#include "algorithm/pathfind.h"
#include "process/core_node.h"
#include "process/route_control.h"

#define PROTOCOL "routes"
#define CONFIG "routes"

int driver(int argc, char **argv) {
  LocalNode this;
  init_this_node(&this, PROTOCOL, CONFIG, HEARTBEAT_TIMEOUT);

  int id = get_node_id("routes");
  if (id != 2)
    return 1;

  struct hop_dest next_hops[MAX_NODE_NUM];
  memset(next_hops, 0, sizeof next_hops);
  next_hops[0].next_hop = 1;
  next_hops[0].dest_ip = inet_addr("10.0.0.10");
  next_hops[1].next_hop = -1;
  next_hops[1].dest_ip = inet_addr("0.0.0.0");
  next_hops[2].next_hop = 6;
  next_hops[2].dest_ip = inet_addr("10.0.1.11");
  next_hops[3].next_hop = 6;
  next_hops[3].dest_ip = inet_addr("10.0.1.10");
  next_hops[4].next_hop = -1;
  next_hops[4].dest_ip = inet_addr("0.0.0.0");
  next_hops[5].next_hop = 6;
  next_hops[5].dest_ip = inet_addr("10.0.2.1");
  for (int i = 6; i < MAX_NODE_NUM; i++)
    next_hops[i].next_hop = -1;

  update_routing_table(&this, next_hops);

  next_hops[2].next_hop = -1;
  next_hops[2].dest_ip = inet_addr("10.0.1.11");
  next_hops[3].next_hop = -1;
  next_hops[3].dest_ip = inet_addr("10.0.1.10");
  next_hops[4].next_hop = -1;
  next_hops[4].dest_ip = inet_addr("0.0.0.0");
  next_hops[5].next_hop = -1;
  next_hops[5].dest_ip = inet_addr("0.0.0.0");

  update_routing_table(&this, next_hops);

  return 0;
}

int main(int argc, char *argv[]) {
  set_logfile_name("routes");
  log_f("routes started");
  int daemonise = 0;
  int opt;
  while ((opt = getopt(argc, argv, "d")) != -1) {
    switch (opt) {
    case 'd':
      daemonise = 1;
      break;
    default:
      log_f("routes usage: %s [-d]", argv[0]);
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
