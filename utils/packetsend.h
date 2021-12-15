
#ifndef PACKETSEND_H
#define PACKETSEND_H

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <net/route.h>

#include "logging.h"

#define MAXLINE 1024
#define CONF_FILENAME "n1"

int get_node_id(void) {
  return NULL;
}

char* read_node_conf_file(char* filename) {
  int read_size = -1;
  char* buf = (char*)malloc(sizeof(char) * MAXLINE);
  if (buf == NULL) {
    return NULL;
  }
  FILE* file = fopen(filename, "r");
  if (file == NULL) {
    free(buf);
    return NULL;
  }
  read_size = fread(buf, sizeof(char), MAXLINE, file);
  fclose(file);
  buf[read_size] = '\0';
  if (read_size < MAXLINE) {
    return buf;
  } else {
    free(buf);
    return NULL;
  }
}

int get_neighbours(struct rtentry** entries) {
  /*
   * Parsing example:
   * interface eth0
   * ip address 10.0.0.2/24
   * !
   */
  char* contents = read_node_conf_file(CONF_FILENAME);
  char* pch = NULL;
  strtok(contents, "\n");
  strtok(NULL, " ");
  strtok(NULL, " ");
  pch = strtok(NULL, "/");
  log_f("pch: %s", pch);

  // Get number of neighbours
  int n = 0;
  char* start = contents;
  pch = strsep(&contents, "\n");
  while (pch != NULL) {
    n++;
    pch = strsep(&contents, "\n");
  }

  *entries = (struct rtentry*) malloc(n * sizeof(struct rtentry));

  contents = start;
  int i = 0;
  pch = strsep(&contents, "\n");
  while(pch != NULL) {

    struct rtentry route = (*entries)[i];
    memset( &route, 0, sizeof( route ) );

    struct sockaddr_in *addr = (struct sockaddr_in *)&route.rt_gateway;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = inet_addr( "10.0.2.2" );

    addr = (struct sockaddr_in*) &route.rt_dst;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;

    addr = (struct sockaddr_in*) &route.rt_genmask;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;

    // TODO Add the interface name to the request
    route.rt_flags = RTF_UP | RTF_GATEWAY;
    route.rt_metric = 0;





    pch = strsep(&contents, "\n");
    i++;
  }
  



  inet_pton(AF_INET, pch, &(destaddrs->sin_addr));
  free(start);
}

#endif
