
#include "algorithm/node.h"
#include "algorithm/pathfind.h"
#include "network/capture_pi.h"
#include "process/logging.h"

#define NETMASK inet_addr("255.255.255.0");

struct capture_info *cap_infos;
pcap_dumper_t *dumper;
int dumper_curr_n;
LocalNode *this;
char **down_ifaces;
int n_down_ifaces;

void dump_packet(u_char *args, const struct pcap_pkthdr *header,
                 const u_char *packet) {
  if (!is_capturing) {
    return;
  }
  if ((enum LinkState) * args == LINK_UP) {
    return;
  }
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

char *generate_addresses_on_interface(LocalNode *this, char *iface,
                                      struct hop_dest *next_hops) {
  char no_addresses = 1;
  // Get index of neighbour corresponding to the iface
  int up_idx;
  for (up_idx = 0; up_idx < this->node.n_neighbours; up_idx++) {
    if (strcmp(this->interfaces[up_idx], iface) == 0) {
      break;
    }
  }
  char *fexp = (char *)malloc(1024);
  memset(fexp, 0, 1024);
  strcat(fexp, "( dst net ");
  char first = 1;
  for (int i = 0; i < MAX_NODE_NUM; i++) {
    // Add if the next hop is along the correct interface
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

char *generate_addresses_on_down_interfaces(LocalNode *this,
                                            struct hop_dest *next_hops) {
  char no_addresses = 1;
  char first = 1;
  char *fexp = (char *)malloc(1024);
  memset(fexp, 0, 1024);
  strcat(fexp, "( dst net ");
  for (int i = 0; i < this->node.n_neighbours; i++) {
    if (down_ifaces[i] == NULL) {
      continue;
    }
    int j;
    for (j = 0; j < this->node.n_neighbours; j++) {
      if (strcmp(down_ifaces[i], this->interfaces[j]) == 0) {
        break;
      }
    }
    for (int k = 0; k < MAX_NODE_NUM; k++) {
      // Add if the next hop is along the correct interface
      if (this->node.neighbour_ids[j] == next_hops[k].next_hop) {
        // Hacky way to insert 'or' between IP addresses
        if (!first) {
          strcat(fexp, " or ");
        }
        first = 0;
        no_addresses = 0;
        // Add IP address
        struct in_addr a;
        a.s_addr = next_hops[k].dest_ip & NETMASK;
        strcat(fexp, inet_ntoa(a));
        // Add subnet mask
        strcat(fexp, "/24");
      }
    }
  }
  strcat(fexp, " )\0");
  if (no_addresses) {
    free(fexp);
    return NULL;
  }
  return fexp;
}

char *generate_incoming_filter_exp(LocalNode *this,
                                   struct hop_dest *next_hops) {
  char *incoming = generate_exclude_incoming(this);
  char *outgoing = generate_addresses_on_down_interfaces(this, next_hops);

  if (outgoing != NULL) {
    strcat(incoming, " and ");
    strcat(incoming, outgoing);
    free(outgoing);
  }
  // Filter out DTLSR traffic (heartbeats and LSAs)
  strcat(incoming, " and ( not ( port 45000 or 45001 or 45002 ) )");
  return incoming;
}

void set_filter(struct capture_info *info, struct hop_dest *next_hops) {
  if (info->has_fp) {
    pcap_freecode(&info->fp);
  }
  char *filter_exp = generate_incoming_filter_exp(this, next_hops);
  if (pcap_compile(info->handle, &info->fp, filter_exp, 0, info->net) == -1) {
    log_f("couldn't parse filter %s: %s", filter_exp,
          pcap_geterr(info->handle));
    exit(EXIT_FAILURE);
  }
  if (pcap_setfilter(info->handle, &info->fp) == -1) {
    log_f("couldn't install filter %s: %s", filter_exp,
          pcap_geterr(info->handle));
    exit(EXIT_FAILURE);
  }
  info->has_fp = 1;
  free(filter_exp);
}

void init_dev(struct capture_info *info, char *iface) {
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
  dumper_curr_n = 0;
  down_ifaces = (char **)malloc(this->node.n_neighbours * sizeof(char *));
  memset(down_ifaces, 0, this->node.n_neighbours * sizeof(char *));
  cap_infos = (struct capture_info *)malloc(this->node.n_neighbours *
                                            sizeof(struct capture_info));
  memset(cap_infos, 0, this->node.n_neighbours * sizeof(struct capture_info));
  for (int i = 0; i < this->node.n_neighbours; i++) {
    init_dev(&cap_infos[i], this->interfaces[i]);
  }
  dumper = open_dump();
}

char *get_dumper_filename(int n) {
  char *s = (char *)malloc(16 * sizeof(char));
  sprintf(s, "dump_%d.pcap", n);
  return s;
}

#define STD_IFACE "eth0"

pcap_dumper_t *open_dump(void) {
  char errbuf[PCAP_ERRBUF_SIZE];
  pcap_t *handle;
  handle = pcap_open_live(STD_IFACE, SNAP_LEN, 0, 1000, errbuf);
  if (handle == NULL) {
    log_f("couldn't open device %s: %s", STD_IFACE, errbuf);
    exit(EXIT_FAILURE);
  }
  if (pcap_datalink(handle) != DLT_EN10MB) {
    log_f("%s is not ethernet", STD_IFACE);
    exit(EXIT_FAILURE);
  }
  char *filename = get_dumper_filename(dumper_curr_n);
  pcap_dumper_t *d = pcap_dump_open(handle, filename);
  free(filename);
  pcap_close(handle);
  return d;
}

void capture_start_iface(char *down_iface, struct hop_dest *next_hops) {
  is_capturing = 1;
  // Add iface to down_ifaces array
  for (int i = 0; i < this->node.n_neighbours; i++) {
    if (down_ifaces[i] == NULL) {
      down_ifaces[i] = down_iface;
      break;
    }
  }
  // Put downed link into the list
  for (int i = 0; i < this->node.n_neighbours; i++) {
    set_filter(&cap_infos[i], next_hops);
  }
  n_down_ifaces++;
}

void capture_end_iface(char *up_iface, struct hop_dest *next_hops) {
  // Ignore if this is the first time the links have come up
  // Annoying edge case
  if (n_down_ifaces == 0) return;
  for (int i = 0; i < this->node.n_neighbours; i++) {
    if (strcmp(up_iface, down_ifaces[i]) == 0) {
      down_ifaces[i] = NULL;
      break;
    }
  }
  n_down_ifaces--;
  // If all links are now up, stop pcap listeners
  if (n_down_ifaces == 0) {
    is_capturing = 0;
    for (int i = 0; i < this->node.n_neighbours; i++) {
      pcap_breakloop(cap_infos[i].handle);
    }
  }
  capture_packets();
  pcap_dump_flush(dumper);
  // capture_replay_iface(up_iface, next_hops);
  // capture_remove_replayed_packets(up_iface, next_hops);
}

void capture_packets(void) {
  for (int i = 0; i < this->node.n_neighbours; i++) {
    // if (this->node.link_statuses[i] == LINK_UP) {
    pcap_dispatch(cap_infos[i].handle, -1, dump_packet,
                  (u_char *)&this->node.link_statuses[i]);
    // }
  }
  pcap_dump_flush(dumper);
}

// tcpdump -r dump.pcap -w- 'udp port 1234' | tcpreplay -ieth0 -
char *generate_replay_command(LocalNode *this, char *up_iface,
                              struct hop_dest *next_hops) {
  char *filename = get_dumper_filename(dumper_curr_n);
  char *fexp = generate_addresses_on_interface(this, up_iface, next_hops);
  char *cmd = (char *)malloc(80 + 32 * this->node.n_neighbours);
  // Append '2>&1' to output error stream
  sprintf(cmd, "tcpdump -U -r '%s' -w- '%s' | tcpreplay --topspeed -i%s -",
          filename, fexp, up_iface);
  free(fexp);
  free(filename);
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
char *generate_filter_command(LocalNode *this, char *up_iface,
                              char *old_filename, char *new_filename,
                              struct hop_dest *next_hops) {
  char *fexp = generate_addresses_on_interface(this, up_iface, next_hops);
  char *cmd = (char *)malloc(80 + 32 * this->node.n_neighbours);
  // Append '2>&1' to output error stream
  sprintf(cmd, "tcpdump -r '%s' -w '%s' '( not %s )'", old_filename,
          new_filename, fexp);
  free(fexp);
  return cmd;
}

void capture_remove_replayed_packets(char *up_iface,
                                     struct hop_dest *next_hops) {
  FILE *fp;
  pcap_dump_close(dumper);
  char *old_filename = get_dumper_filename(dumper_curr_n);
  char *new_filename = get_dumper_filename(dumper_curr_n + 1);
  char *cmd = generate_filter_command(this, up_iface, old_filename,
                                      new_filename, next_hops);
  fp = popen(cmd, "r");
  if (fp == NULL) {
    log_f("failed to run filter command: %s", cmd);
    return;
  }
  pclose(fp);
  sprintf(cmd, "rm '%s'", old_filename);
  fp = popen(cmd, "r");
  if (fp == NULL) {
    log_f("failed to run rename command: %s", cmd);
    return;
  }
  pclose(fp);
  free(old_filename);
  free(new_filename);
  dumper_curr_n++;
  dumper = open_dump();
}
