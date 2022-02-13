
#ifndef CAPTURE_PI_H
#define CAPTURE_PI_H

#include "capture.h"

#define PCAP_FILENAME "dump.pcap"

#define SNAP_LEN 1518
#define SIZE_ETHERNET 14
#define ETHER_ADDR_LEN 6
#define SIZE_UDP 8

struct sniff_ethernet {
  u_char ether_dhost[ETHER_ADDR_LEN];
  u_char ether_shost[ETHER_ADDR_LEN];
  u_short ether_type;
};

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

typedef u_int tcp_seq;

struct sniff_tcp {
	u_short th_sport;
	u_short th_dport;
	tcp_seq th_seq;
	tcp_seq th_ack;
	u_char  th_offx2;
#define TH_OFF(th) (((th)->th_offx2 & 0xf0) >> 4)
	u_char  th_flags;
	#define TH_FIN 0x01
	#define TH_SYN 0x02
	#define TH_RST 0x04
	#define TH_PUSH 0x08
	#define TH_ACK 0x10
	#define TH_URG 0x20
	#define TH_ECE 0x40
	#define TH_CWR 0x80
	#define TH_FLAGS (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
	u_short th_win;
	u_short th_sum;
	u_short th_urp;
};

struct sniff_udp {
  u_short udp_sport;
  u_short udp_dport;
  u_short udp_length;
  u_short udp_checksum;
};

void dump_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

char *generate_exclude_incoming(LocalNode *this);

char *generate_addresses_on_interface(LocalNode *this, char *iface, struct hop_dest *next_hops);

char *generate_incoming_filter_exp(LocalNode *this, char *iface, struct hop_dest *next_hops);

void set_filter(struct capture_info *info, char *iface, struct hop_dest *next_hops);

void init_dev(struct capture_info *info, char *iface);

pcap_dumper_t *open_dump();

char *generate_replay_command(LocalNode *this, char *up_iface, struct hop_dest *next_hops);

void capture_replay_iface(char *up_iface, struct hop_dest *next_hops);

char *generate_remove_command(LocalNode *this, char *up_iface, struct hop_dest *next_hops);

void capture_remove_replayed_packets(char *up_iface, struct hop_dest *next_hops);

#endif
