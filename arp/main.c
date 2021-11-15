
#include <stdlib.h>
#include <arpa/inet.h>

#include "logging.h"
#include "daemonise.h"
#include "arp.h"

int driver(int argc, char** argv) {
  long* arp;
  char ip[16];
  while (1) {
    sleep(3);
    u_int8_t n = get_neighbour_ips(&arp);
    for (u_int8_t i = 0; i < n; i++) {
      inet_ntop(AF_INET, &(arp[i]), ip, INET_ADDRSTRLEN);
      ip[15] = '\0';
      log_f("%s", ip);
    }
    free(arp);
  }
}

int main(int argc, char* argv[]) {
  set_logfile_name("arp-monitor");
  log_f("ARP Monitor Started");
	int daemonise = 0;
  int opt;
  while ((opt = getopt(argc, argv, "d")) != -1) {
    switch (opt) {
      case 'd': daemonise = 1; break;
      default:
        log_f("Server usage: %s [-d]", argv[0]);
        exit(EXIT_FAILURE);
    }
  }
  if (daemonise) {
    make_daemon();
    log_f("Daemonisation successful");
  }
  return driver(argc, argv);
}
