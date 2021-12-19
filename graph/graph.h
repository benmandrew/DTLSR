
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

#define PORT 8080
#define HB_SIZE sizeof(int)
#define HEARTBEAT_T 5
#define HEARTBEAT_TIMEOUT HEARTBEAT_T + 3

typedef struct Graph {
  Node* nodes;
  size_t n_nodes;
} Graph;

int get_n_neighbours(void);

void init_node(void);

void register_heartbeat(long source_addr);

void timeout_heartbeat(int active_fd);

int* init_fds(int sockfd);

#endif
