
#ifndef GRAPH_H
#define GRAPH_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "algorithm/node.h"
#include "algorithm/pathfind.h"
#include "network/packetsend.h"
#include "process/daemonise.h"
#include "process/fd_event.h"
#include "process/logging.h"

struct hop_dest;

struct rtentry *routes;

// File descriptors for operation of Link-State graph + sockets
typedef struct LSFD {
  // Heartbeat
  int hb_sock;
  // LSA receiving
  int lsa_rec_sock;
  // LSA sending
  int lsa_snd_sock;
  // LS metric recomputation
  Timer lsa_snd_timer;
  // Event system file descriptors
  int *event_fds;
  int n_event_fds;
} LSFD;

void graph_init(Node *graph);

char receive_heartbeat(Node *graph, LocalNode *this, LSFD *fds, struct hop_dest *next_hops);

void local_node_update_metrics(LocalNode *this, unsigned long long now);

void update_global_this(Node *graph, LocalNode *this);

char timeout_heartbeat(Node *graph, LocalNode *this, int active_fd, LSFD *fds);

char receive_lsa(Node *graph, LocalNode *this, LSFD *fds);

void send_lsa_except(Node *graph, LocalNode *this, LSFD *fds, long source_addr);

void send_lsa(Node *graph, LocalNode *this, LSFD *fds);

#endif
