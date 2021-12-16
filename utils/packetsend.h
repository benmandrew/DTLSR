
#ifndef PACKETSEND_H
#define PACKETSEND_H

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <net/route.h>

#include "logging.h"
#include "fileio.h"

#define MAXLINE 1024
#define CONFIG_PATH "/home/ben/projects/routing/configs"

char* _read_node_conf_file(char* protocol) {
  log_f("%s.id", protocol);
  // Get node ID
  char filename[FILENAME_MAX];
  sprintf(filename, "%s.id", protocol);
  log_f("%s", filename);
  char* node_id = read_file(filename);
  // Use node ID to find the corresponding config file
  sprintf(filename, "%s/%s/n%s", CONFIG_PATH, protocol, node_id);
  free(node_id);
  log_f("%s", filename);
  return read_file(filename);
}

int _parse_neighbour(char* pch, struct rtentry* entry) {
  char* saved;
  char* interface = strtok_r(pch, " - ", &saved);
  char* address = strtok_r(NULL, " - ", &saved);

  log_f("%s %s", interface, address);
  struct rtentry route = *entry;
  memset(&route, 0, sizeof(route));

  struct sockaddr_in *addr = (struct sockaddr_in*)&route.rt_gateway;
  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = inet_addr(address);

  addr = (struct sockaddr_in*) &route.rt_dst;
  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = inet_addr(address);

  addr = (struct sockaddr_in*) &route.rt_genmask;
  addr->sin_family = AF_INET;
  addr->sin_addr.s_addr = INADDR_ANY;
  
  route.rt_dev = interface;
  route.rt_flags = RTF_UP | RTF_GATEWAY;
  route.rt_metric = 0;
}

/*
 * Parsing example:
 * eth0 - 10.0.0.1
 * eth1 - 10.0.1.2
 */
int get_neighbours(struct rtentry** entries, char* protocol) {
  char* contents = _read_node_conf_file(protocol);
  // Get number of neighbours
  int n = 0, i = 0;
  while (contents[i] != '\0') {
    if (contents[i] == '\n') n++;
    i++;
  }
  *entries = (struct rtentry*)malloc(n * sizeof(struct rtentry));
  // Save start so contents can be freed
  char* start = contents;
  i = 0;
  char* saved;
  char* pch = strtok_r(contents, "\n", &saved);
  while(pch != NULL) {
    _parse_neighbour(pch, &((*entries)[i]));
    pch = strtok_r(NULL, "\n", &saved);
    i++;
  }
  free(start);
  return n;
}

#endif
