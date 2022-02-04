
#include "algorithm/node.h"
#include "algorithm/pathfind.h"
#include "process/logging.h"
#include "network/capture_pi.h"

struct capture_info *cap_infos;
pcap_dumper_t **dumpers;
LocalNode *this;
char **down_ifaces;
int n_down_ifaces;

void dump_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
  const struct sniff_ip *ip = (struct sniff_ip *)(packet + SIZE_ETHERNET);
  int size_ip = IP_HL(ip) * 4;
  if (size_ip < 20) {
    // log_f("invalid IP header length: %u bytes", size_ip);
    return;
  }
  pcap_dump((u_char *)dumpers[0], header, packet);
}

char *generate_addresses_on_interface(LocalNode *this, char *iface, struct hop_dest *next_hops) {
  log_f("1");
  // Get index of neighbour corresponding to the iface
  int up_idx;
  for (up_idx = 0; up_idx < this->node.n_neighbours; up_idx++) {
    if (strcmp(this->interfaces[up_idx], iface) == 0) {
      break;
    }
  }
  log_f("2");
  char *fexp = (char *)malloc(32 * this->node.n_neighbours);
  memset(fexp, 0, 32 * this->node.n_neighbours);
  strcat(fexp, "dst net ");
  char first = 1;
  for (int i = 0; i < MAX_NODE_NUM; i++) {
    log_f("3");
    if (next_hops[i].next_hop == this->node.neighbour_ids[up_idx]) {
      // Hacky way to insert 'or' between IP addresses
      if (!first) {
        strcat(fexp, " or ");
      }
      first = 0;
      // Add IP address
      struct in_addr a;
      a.s_addr = next_hops[i].dest_ip;
      strcat(fexp, inet_ntoa(a));
      // Add subnet mask
      strcat(fexp, "/24");
    }
  }
  log_f("4");
  strcat(fexp, "\0");
  return fexp;
}

// We want to exclude packets meant specifically for us
// not ( dst host ip1 or ip2 or ip3 )
char *generate_incoming_filter_exp(LocalNode *this) {
  log_f("5");
  char *s = (char *)malloc(32 * this->node.n_neighbours);
  memset(s, 0, 32 * this->node.n_neighbours);
  strcat(s, "not ( dst net ");
  for (int i = 0; i < this->node.n_neighbours; i++) {
    log_f("6");
    struct in_addr a;
    a.s_addr = this->node.source_ips[i];
    strcat(s, inet_ntoa(a));
    if (i != this->node.n_neighbours - 1) {
      strcat(s, " or ");
    }
  }
  log_f("7");
  strcat(s, " )\0");
  return s;
}

void set_filter(struct capture_info *info) {
  if (info->has_fp) {
    pcap_freecode(&info->fp);
  }
  char *filter_exp = generate_incoming_filter_exp(this);
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
  set_filter(info);
}

void capture_init(LocalNode *node) {
  is_capturing = 0;
  this = node;
  n_down_ifaces = 0;
  down_ifaces = (char **)malloc(this->node.n_neighbours * sizeof(char *));
  memset(down_ifaces, 0, this->node.n_neighbours * sizeof(char *));
  cap_infos = (struct capture_info *)malloc(this->node.n_neighbours * sizeof(struct capture_info));
  memset(cap_infos, 0, this->node.n_neighbours * sizeof(struct capture_info));
  dumpers = (pcap_dumper_t **)malloc(this->node.n_neighbours * sizeof(pcap_dumper_t *));
  for (int i = 0; i < this->node.n_neighbours; i++) {
    init_dev(&cap_infos[i], this->interfaces[i]);
  }
}

void capture_start_iface(char *down_iface) {
  is_capturing = 1;
  // Create the pcap file for the outgoing downed link
  for (int i = 0; i < this->node.n_neighbours; i++) {
    if (strcmp(this->interfaces[i], down_iface) == 0) {
      char filename[24];
      sprintf(filename, "%s.pcap", down_iface);
      dumpers[i] = pcap_dump_open(cap_infos[i].handle, filename);
    }
  }
  // Put downed link into the list
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
  // Close the pcap file for the newly up outgoing interface
  for (int i = 0; i < this->node.n_neighbours; i++) {
    log_f("HELLO");
    if (down_ifaces[i] != NULL && strcmp(down_ifaces[i], up_iface) == 0) {
      down_ifaces[i] = NULL;
      pcap_dump_close(dumpers[i]);
      break;
    }
    log_f("HELLO AGAIN");
  }
  capture_replay_iface(up_iface, next_hops);
}

void capture_packets(void) {
  for (int i = 0; i < this->node.n_neighbours; i++) {
    pcap_dispatch(cap_infos[i].handle, -1, dump_packet, NULL);
  }
}

// tcpdump -r dump.pcap -w- 'udp port 1234' | tcpreplay -ieth0 - 
char *generate_replay_command(LocalNode *this, char *up_iface, struct hop_dest *next_hops) {
  log_f("8");
  char *fexp = generate_addresses_on_interface(this, up_iface, next_hops);
  char *cmd = (char *)malloc(80 + 32 * this->node.n_neighbours);
  // Append for error stream: 2>&1
  sprintf(cmd, "tcpdump -U -r %s.pcap -w- '%s' | tcpreplay --topspeed -i%s -", up_iface, fexp, up_iface);
  free(fexp);
  log_f("9");
  return cmd;
}

void capture_replay_iface(char *up_iface, struct hop_dest *next_hops) {
  FILE *fp;
  // char path[1035];
  char *cmd = generate_replay_command(this, up_iface, next_hops);
  fp = popen(cmd, "r");
  if (fp == NULL) {
    log_f("failed to run replay command: %s", cmd);
    return;
  }
  // while (fgets(path, sizeof(path), fp) != NULL) {
  //   log_f("%s", path);
  // }
  pclose(fp);
  free(cmd);
}
