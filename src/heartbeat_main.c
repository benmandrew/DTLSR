
#include "algorithm/def.h"
#include "filenames.h"
#include "network/packetsend.h"
#include "process/core_node.h"
#include "process/daemonise.h"
#include "process/fd_event.h"
#include "process/logging.h"

#define PROTOCOL "heartbeat"

int driver(int argc, char **argv) {
  LocalNode this;
  // Creating socket file descriptor
  int sockfd = get_socket();
  // Read neighbours
  init_this_node(&this, PROTOCOL, CONFIG, HEARTBEAT_TIMEOUT);
  struct rtentry *routes = get_routes(&this);
  // Create heartbeat timer
  Timer timer = event_timer_append(HEARTBEAT_T, 0);

  int active_fd;
  // Event loop
  while (1) {
    if ((active_fd = event_wait(&timer.fd, 1)) < 0) {
      continue;
    }
    if (active_fd == timer.fd) {
      event_timer_reset(&timer);
      // Send heartbeat to every neighbour
      for (int i = 0; i < this.node.n_neighbours; i++) {
        struct sockaddr_in *neighbour_addr =
            (struct sockaddr_in *)&(routes[i].rt_dst);
        neighbour_addr->sin_port = htons(HB_PORT);
        int addr_len = sizeof(*neighbour_addr);
        // Send 'addr_len' just in case 'sendto'
        // dereferences it even with zero length
        sendto(sockfd, &addr_len, sizeof(addr_len), MSG_CONFIRM,
               (const struct sockaddr *)neighbour_addr, addr_len);
      }
      log_f("heartbeats sent");
    }
  }
  close(sockfd);
  close(timer.fd);
  return 0;
}

int main(int argc, char *argv[]) {
  set_logfile_name("hbt");
  log_f("heartbeat started");
  int daemonise = 0;
  int opt;
  while ((opt = getopt(argc, argv, "d")) != -1) {
    switch (opt) {
    case 'd':
      daemonise = 1;
      break;
    default:
      log_f("heartbeat usage: %s [-d]", argv[0]);
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
