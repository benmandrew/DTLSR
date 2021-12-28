
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

LocalNode this;

// Link-State Graph representation
typedef struct Graph {
  Node* nodes;
  size_t n_nodes;
} Graph;

#define N_EVENT_SOCKS 2

// Sockets required for Graph + event file descriptors
typedef struct LSSockets {
  // Heartbeat
	int hb_sock;
  // LSA receiving
  int lsa_rec_sock;
  // LSA sending
  int lsa_snd_sock;
  // Event system file descriptors
  int* event_fds;
  int n_event_fds;
} LSSockets;


void init_node(void);

void receive_heartbeat(LSSockets* socks);

void timeout_heartbeat(int active_fd, LSSockets* socks);

void aggregate_fds(LSSockets* socks, int n_sockfds);

void receive_lsa(LSSockets* socks);

void send_lsa_except(LSSockets* socks, long source_addr);

void send_lsa(LSSockets* socks);

#endif
