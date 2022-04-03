
#ifndef CAPTURE_H
#define CAPTURE_H

#include <pcap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "algorithm/node.h"

typedef struct local_node LocalNode;
struct hop_dest;

typedef unsigned int u_int;
typedef unsigned short u_short;
typedef unsigned char u_char;

struct capture_info {
  char *dev;
  pcap_t *handle;
  struct bpf_program fp;
  char has_fp;
  bpf_u_int32 net;
};

char is_capturing;

void capture_init(LocalNode *node, struct hop_dest *next_hops);

void capture_packets(void);

void capture_replay_iface(char *up_iface, struct hop_dest *next_hops);

void capture_start_iface(char *down_iface, struct hop_dest *next_hops);

void capture_end_iface(char *up_iface, struct hop_dest *next_hops);

void capture_remove_replayed_packets(char *up_iface, struct hop_dest *next_hops);

#endif
