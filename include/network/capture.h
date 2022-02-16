
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

#define SIZE_ETHERNET 14

typedef struct local_node LocalNode;
struct hop_dest;

typedef unsigned int u_int;
typedef unsigned short u_short;
typedef unsigned char u_char;

struct sniff_ip {
  u_char ip_vhl; // version << 4 | header length >> 2
  u_char ip_tos;
  u_short ip_len; // total length
  u_short ip_id;
  u_short ip_off;
#define IP_RF 0x8000
#define IP_DF 0x4000
#define IP_MF 0x2000
#define IP_OFFMASK 0x1fff
  u_char ip_ttl;
  u_char ip_p;
  u_short ip_sum;
  struct in_addr ip_src, ip_dst;
};
#define IP_HL(ip) (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip) (((ip)->ip_vhl) >> 4)
// Switch byte order
#define IP_LEN(len) ((u_short)((len & 0xFF00) >> 8) | ((len & 0x00FF) << 8))

#define IP_PTR(packet) ((struct sniff_ip *)(packet + SIZE_ETHERNET))
#define PACKET_LEN(ip_len) (IP_LEN(ip_len) + SIZE_ETHERNET)
#define PAYLOAD_PTR(ip) ((u_char *)((ip) + (IP_HL(ip) * 4)))
#define PAYLOAD_LEN(ip) (IP_LEN((ip)->ip_len) - (IP_HL(ip) * 4))

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

#endif
