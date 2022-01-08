
#include <stdio.h>
#include <stdlib.h>

#include "process/daemonise.h"
#include "process/fd_event.h"

// Control-level logging
#include "process/logging.h"
// Data-level output
#include "process/perf_logging.h"

int driver(int argc, char **argv) {
  FILE *fp;
  char path[160];
  fp = popen("/usr/bin/iperf3", "r");
  if (fp == NULL) {
    printf("Failed to run command\n" );
    exit(1);
  }
  // Read the output a line at a time
  while (fgets(path, sizeof(path), fp) != NULL) {
    printf("%s", path);
  }
  pclose(fp);
}

int main(int argc, char *argv[]) {
  set_logfile_name("iperf");
  set_perf_logfile_path("~/iperf");
  log_f("iperf started");
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
