
#include "algorithm/node.h"
#include "algorithm/pathfind.h"
#include "process/logging.h"
#include "network/capture_pi.h"

#define NETMASK inet_addr("255.255.255.0");

struct capture_info *cap_infos;
pcap_dumper_t *dumper;
LocalNode *this;
char **down_ifaces;
int n_down_ifaces;

void dump_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
  const struct sniff_ip *ip = (struct sniff_ip *)(packet + SIZE_ETHERNET);
  int size_ip = IP_HL(ip) * 4;
  if (size_ip < 20) {
    return;
  }
  pcap_dump((u_char *)dumper, header, packet);
}

// We want to exclude packets meant specifically for us
char *generate_exclude_incoming(LocalNode *this) {
  char *fexp = (char *)malloc(32 * (this->node.n_neighbours + MAX_NODE_NUM));
  memset(fexp, 0, 32 * this->node.n_neighbours);
  strcat(fexp, "( not ( dst net ");
  for (int i = 0; i < this->node.n_neighbours; i++) {
    struct in_addr a;
    a.s_addr = this->node.source_ips[i];
    strcat(fexp, inet_ntoa(a));
    if (i != this->node.n_neighbours - 1) {
      strcat(fexp, " or ");
    }
  }
  strcat(fexp, " ) )\0");
  return fexp;
}

char *generate_addresses_on_interface(LocalNode *this, char *iface, struct hop_dest *next_hops) {
  char no_addresses = 1;
  // Get index of neighbour corresponding to the iface
  int up_idx;
  for (up_idx = 0; up_idx < this->node.n_neighbours; up_idx++) {
    if (strcmp(this->interfaces[up_idx], iface) == 0) {
      break;
    }
  }
  char *fexp = (char *)malloc(32 * this->node.n_neighbours);
  memset(fexp, 0, 32 * this->node.n_neighbours);
  strcat(fexp, "( dst net ");
  char first = 1;
  for (int i = 0; i < MAX_NODE_NUM; i++) {
    if (next_hops[i].next_hop == this->node.neighbour_ids[up_idx]) {
      // Hacky way to insert 'or' between IP addresses
      if (!first) {
        strcat(fexp, " or ");
      }
      first = 0;
      no_addresses = 0;
      // Add IP address
      struct in_addr a;
      a.s_addr = next_hops[i].dest_ip & NETMASK;
      strcat(fexp, inet_ntoa(a));
      // Add subnet mask
      strcat(fexp, "/24");
    }
  }
  strcat(fexp, " )\0");
  if (no_addresses) {
    free(fexp);
    return NULL;
  }
  return fexp;
}

char *generate_incoming_filter_exp(LocalNode *this, char *iface, struct hop_dest *next_hops) {
  char *incoming = generate_exclude_incoming(this);
  char *outgoing = generate_addresses_on_interface(this, iface, next_hops);

  if (outgoing != NULL) {
    strcat(incoming, " and ");
    strcat(incoming, outgoing);
    free(outgoing);
  }
  return incoming;
}

void set_filter(struct capture_info *info, char *iface, struct hop_dest *next_hops) {
  if (info->has_fp) {
    pcap_freecode(&info->fp);
  }
  char *filter_exp = generate_incoming_filter_exp(this, iface, next_hops);
  if (pcap_compile(info->handle, &info->fp, filter_exp, 0, info->net) == -1) {
    log_f("couldn't parse filter %s: %s", filter_exp, pcap_geterr(info->handle));
    exit(EXIT_FAILURE);
  }
  if (pcap_setfilter(info->handle, &info->fp) == -1) {
    log_f("couldn't install filter %s: %s", filter_exp, pcap_geterr(info->handle));
    exit(EXIT_FAILURE);
  }
  info->has_fp = 1;
  free(filter_exp);
}

void init_dev(struct capture_info *info, char *iface, struct hop_dest *next_hops) {
  char errbuf[PCAP_ERRBUF_SIZE];
  bpf_u_int32 mask;
  info->dev = iface;
  if (pcap_lookupnet(iface, &info->net, &mask, errbuf) == -1) {
    log_f("couldn't get netmask for device %s: %s\n", iface, errbuf);
    info->net = 0;
    mask = 0;
  }
  info->handle = pcap_open_live(iface, SNAP_LEN, 0, 1000, errbuf);
  if (info->handle == NULL) {
    log_f("couldn't open device %s: %s", iface, errbuf);
    exit(EXIT_FAILURE);
  }
  // make sure we're capturing on an Ethernet device
  if (pcap_datalink(info->handle) != DLT_EN10MB) {
    log_f("%s is not ethernet", iface);
    exit(EXIT_FAILURE);
  }
  if (pcap_setnonblock(info->handle, 1, errbuf) == -1) {
    log_f("couldn't set handle nonblocking: %s", pcap_geterr(info->handle));
    exit(EXIT_FAILURE);
  }
}

void capture_init(LocalNode *node, struct hop_dest *next_hops) {
  this = node;
  is_capturing = 0;
  n_down_ifaces = 0;
  down_ifaces = (char **)malloc(this->node.n_neighbours * sizeof(char *));
  memset(down_ifaces, 0, this->node.n_neighbours * sizeof(char *));
  cap_infos = (struct capture_info *)malloc(this->node.n_neighbours * sizeof(struct capture_info));
  memset(cap_infos, 0, this->node.n_neighbours * sizeof(struct capture_info));
  for (int i = 0; i < this->node.n_neighbours; i++) {
    init_dev(&cap_infos[i], this->interfaces[i], next_hops);
  }
  dumper = pcap_dump_open(cap_infos[0].handle, PCAP_FILENAME);
}

void capture_start_iface(char *down_iface, struct hop_dest *next_hops) {
  is_capturing = 1;
  // Put downed link into the list
  for (int i = 0; i < this->node.n_neighbours; i++) {
    set_filter(&cap_infos[i], down_iface, next_hops);
  }
  for (int i = 0; i < this->node.n_neighbours; i++) {
    if (down_ifaces[i] == NULL) {
      down_ifaces[i] = down_iface;
      break;
    }
  }
  n_down_ifaces++;
}

void capture_end_iface(char* up_iface, struct hop_dest *next_hops) {
  // Ignore if this is the first time the links have come up
  // Annoying edge case
  if (n_down_ifaces == 0)
    return;
  n_down_ifaces--;
  // If all links are now up, stop pcap listeners
  if (n_down_ifaces == 0) {
    is_capturing = 0;
    for (int i = 0; i < this->node.n_neighbours; i++) {
      pcap_breakloop(cap_infos[i].handle);
    }
  }
  capture_replay_iface(up_iface, next_hops);
  capture_remove_replayed_packets(up_iface, next_hops);
}

void capture_packets(void) {
  log_f("dump");
  for (int i = 0; i < this->node.n_neighbours; i++) {
    pcap_dispatch(cap_infos[i].handle, -1, dump_packet, NULL);
  }
}

// tcpdump -r dump.pcap -w- 'udp port 1234' | tcpreplay -ieth0 - 
char *generate_replay_command(LocalNode *this, char *up_iface, struct hop_dest *next_hops) {
  char *fexp = generate_addresses_on_interface(this, up_iface, next_hops);
  char *cmd = (char *)malloc(80 + 32 * this->node.n_neighbours);
  // Append '2>&1' to output error stream
  sprintf(cmd, "tcpdump -U -r 'dump.pcap' -w- '%s' | tcpreplay --topspeed -i%s -", fexp, up_iface);
  free(fexp);
  return cmd;
}

void capture_replay_iface(char *up_iface, struct hop_dest *next_hops) {
  FILE *fp;
  char *cmd = generate_replay_command(this, up_iface, next_hops);
  fp = popen(cmd, "r");
  if (fp == NULL) {
    log_f("failed to run replay command: %s", cmd);
    return;
  }
  pclose(fp);
  free(cmd);
}

// tcpdump -r dump.pcap -w dump.pcap 'not ( udp port 1234 )'
char *generate_remove_command(LocalNode *this, char *up_iface, struct hop_dest *next_hops) {
  char *fexp = generate_addresses_on_interface(this, up_iface, next_hops);
  char *cmd = (char *)malloc(80 + 32 * this->node.n_neighbours);
  // Append '2>&1' to output error stream
  sprintf(cmd, "tcpdump -r 'dump.pcap' -w 'dump_tmp.pcap' '( not %s )'", fexp);
  free(fexp);
  return cmd;
}

void capture_remove_replayed_packets(char *up_iface, struct hop_dest *next_hops) {
  FILE *fp;
  pcap_dump_close(dumper);
  char *cmd = generate_remove_command(this, up_iface, next_hops);
  fp = popen(cmd, "r");
  if (fp == NULL) {
    log_f("failed to run remove command: %s", cmd);
    return;
  }
  free(cmd);
  pclose(fp);
  cmd = "rm dump.pcap";
  fp = popen(cmd, "r");
  if (fp == NULL) {
    log_f("failed to run rename command: %s", cmd);
    return;
  }
  pclose(fp);
  cmd = "mv dump_tmp.pcap dump.pcap";
  fp = popen(cmd, "r");
  if (fp == NULL) {
    log_f("failed to run rename command: %s", cmd);
    return;
  }
  pclose(fp);
  dumper = pcap_dump_open(cap_infos[0].handle, "dump.pcap");
}
