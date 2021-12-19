
#ifndef GRAPH_H
#define GRAPH_H

#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "logging.h"
#include "daemonise.h"
#include "packetsend.h"
#include "fd_event.h"
#include "node.h"

typedef struct Graph {
  Node* nodes;
  size_t n_nodes;
} Graph;

int get_n_neighbours(void);

void init_node(void);

char register_heartbeat(long source_addr);

char timeout_heartbeat(int active_fd);

int* init_fds(int* sockfds, int n_sockfds);

#endif
