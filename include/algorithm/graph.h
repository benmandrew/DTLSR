
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
#include "network/packetsend.h"
#include "process/daemonise.h"
#include "process/fd_event.h"
#include "process/logging.h"

struct rtentry *routes;

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
  int *event_fds;
  int n_event_fds;
} LSSockets;

void graph_init(Node *graph);

char receive_heartbeat(Node *graph, LocalNode *this, LSSockets *socks);

void update_global_this(Node *graph, Node *this);

char merge_in(Node *these, Node *others);

char timeout_heartbeat(Node *graph, LocalNode *this, int active_fd,
                       LSSockets *socks);

void aggregate_fds(LocalNode *this, LSSockets *socks, int n_sockfds);

char receive_lsa(Node *graph, LocalNode *this, LSSockets *socks);

void send_lsa_except(Node *graph, LocalNode *this, LSSockets *socks,
                     long source_addr);

void send_lsa(Node *graph, LocalNode *this, LSSockets *socks);

#endif
