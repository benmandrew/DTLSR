
#include "algorithm/node.h"
#include "process/logging.h"
#include "network/capture_pi.h"

void dump_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
  const struct sniff_ip *ip = (struct sniff_ip *)(packet + SIZE_ETHERNET);
  int size_ip = IP_HL(ip) * 4;
  if (size_ip < 20) {
    log_f("invalid IP header length: %u bytes", size_ip);
    return;
  }
  // TODO:
  // Check that dst IP is not one of our interfaces
  log_f("From: %s\n", inet_ntoa(ip->ip_src));
  log_f("  To: %s\n", inet_ntoa(ip->ip_dst));











  pcap_dump(args, header, packet);
  return;
}

struct capture_info *cap_infos;
struct pcap_dumper_t **dumpers;
int n_ifaces;

int n_down_ifaces;

char *generate_filter_exp(Node *this) {
  // not ( ip1 and ip2 and ip3 )
  char *s = (char *)malloc(32 * this->n_neighbours);
  strcat(s, "not ( ");
  for (int i = 0; i < this->n_neighbours; i++) {
    struct in_addr a;
    a.s_addr = this->source_ips;
    strcat(s, inet_ntoa(a));
    if (i != this->n_neighbours - 1) {
      strcat(s, " and ");
    }
  }
  strcat(s, " )\0");
  return s;
}

void set_filter(struct capture_info *info, bpf_u_int32 net, Node *this) {
  if (info->has_fp) {
    pcap_freecode(&info->fp);
  }
  char *filter_exp = generate_filter_exp(this);
  if (pcap_compile(info->handle, &info->fp, filter_exp, 0, net) == -1) {
    log_f("couldn't parse filter %s: %s",
            filter_exp, pcap_geterr(info->handle));
    exit(EXIT_FAILURE);
  }
  if (pcap_setfilter(info->handle, &info->fp) == -1) {
    log_f("couldn't install filter %s: %s",
            filter_exp, pcap_geterr(info->handle));
    exit(EXIT_FAILURE);
  }
  info->has_fp = 1;
  free(filter_exp);
}

void init_dev(struct capture_info *info, char *iface, Node *this) {
  char errbuf[PCAP_ERRBUF_SIZE];
  bpf_u_int32 mask;
  bpf_u_int32 net;
  cap_infos->dev = iface;
  if (pcap_lookupnet(iface, &net, &mask, errbuf) == -1) {
    log_f("Couldn't get netmask for device %s: %s\n",
            iface, errbuf);
    net = 0;
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
  set_filter(info, net, this);
  if (pcap_setnonblock(info->handle, 1, errbuf) == -1) {
    log_f("couldn't set handle nonblocking: %s", pcap_geterr(info->handle));
    exit(EXIT_FAILURE);
  }
}

void capture_init(char **ifaces, int n, Node *this) {
  n_ifaces = n;
  n_down_ifaces = 0;
  cap_infos = (struct capture_info *)malloc(n * sizeof(struct capture_info));
  memset(cap_infos, 0, n * sizeof(struct capture_info));
  dumpers = (pcap_dumper_t **)malloc(n * sizeof(pcap_dumper_t *));
  for (int i = 0; i < n; i++) {
    init_dev(&cap_infos[i], ifaces[i], this);
    char filename[16];
    sprintf(filename, "%s.pcap", ifaces[i]);
    dumpers[i] = pcap_dump_open(cap_infos[i].handle, filename);
  }
}





void capture_start(char* iface) {











  
  pcap_dispatch(cap_info.handle, -1, dump_packet, (u_char *)cap_info.dumper);
  return cap_info;
}

void capture_flush(struct capture_info cap_info) {
  pcap_dump_flush(cap_info.dumper);
  pcap_dump_close(cap_info.dumper);
  pcap_close(cap_info.handle);
}
