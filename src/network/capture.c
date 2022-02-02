
#include "algorithm/node.h"
#include "process/logging.h"
#include "network/capture_pi.h"

struct capture_info *cap_infos;
pcap_dumper_t **dumpers;
LocalNode *this;
char **down_ifaces;
int n_down_ifaces;

void dump_tcp(const struct pcap_pkthdr *header, const u_char *packet, int size_ip) {
  const struct sniff_tcp *tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
  int size_tcp = TH_OFF(tcp) * 4;
  if (size_tcp < 20) {
		// log_f("invalid TCP header length: %u bytes", size_tcp);
		return;
	}
  pcap_dump((u_char *)dumpers[0], header, packet);
}

void dump_udp(const struct pcap_pkthdr *header, const u_char *packet, int size_ip) {
  const struct sniff_udp *udp = (struct sniff_udp*)(packet + SIZE_ETHERNET + size_ip);
  int sport = (int)udp->udp_sport;
  int dport = (int)udp->udp_dport;
  int size_udp = (int)udp->udp_length;
  log_f("udp - %d %d", sport, dport);
  if (size_udp < 8) {
		// log_f("invalid UDP length: %u bytes", size_udp);
		return;
	}
  pcap_dump((u_char *)dumpers[0], header, packet);
}

void dump_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet) {
  const struct sniff_ip *ip = (struct sniff_ip *)(packet + SIZE_ETHERNET);
  int size_ip = IP_HL(ip) * 4;
  if (size_ip < 20) {
    // log_f("invalid IP header length: %u bytes", size_ip);
    return;
  }
  // TODO:
  // Check that dst IP is not one of our interfaces
  // log_f("From: %s", inet_ntoa(ip->ip_src));
  // log_f("To: %s", inet_ntoa(ip->ip_dst));

  switch(ip->ip_p) {
		case IPPROTO_TCP:
			printf("Protocol: TCP\n");
      dump_tcp(header, packet, size_ip);
			break;
		case IPPROTO_UDP:
			printf("Protocol: UDP\n");
      dump_udp(header, packet, size_ip);
			break;
		case IPPROTO_ICMP:
			printf("Protocol: ICMP\n");
			return;
		case IPPROTO_IP:
			printf("Protocol: IP\n");
			return;
		default:
			printf("Protocol: unknown\n");
			return;
	}
  pcap_dump_flush(dumpers[0]);
}

char *generate_filter_exp(void) {
  // not src host ( ip1 and ip2 and ip3 )
  char *s = (char *)malloc(32 * this->node.n_neighbours);
  strcat(s, "not dst host ( ");
  for (int i = 0; i < this->node.n_neighbours; i++) {
    struct in_addr a;
    a.s_addr = this->node.source_ips[i];
    strcat(s, inet_ntoa(a));
    if (i != this->node.n_neighbours - 1) {
      strcat(s, " and ");
    }
  }
  strcat(s, " )\0");
  return s;
}

void set_filter(struct capture_info *info, bpf_u_int32 net) {
  if (info->has_fp) {
    pcap_freecode(&info->fp);
  }
  char *filter_exp = generate_filter_exp();
  if (pcap_compile(info->handle, &info->fp, filter_exp, 0, net) == -1) {
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
  bpf_u_int32 net;
  cap_infos->dev = iface;
  if (pcap_lookupnet(iface, &net, &mask, errbuf) == -1) {
    log_f("couldn't get netmask for device %s: %s\n", iface, errbuf);
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
  set_filter(info, net);
  if (pcap_setnonblock(info->handle, 1, errbuf) == -1) {
    log_f("couldn't set handle nonblocking: %s", pcap_geterr(info->handle));
    exit(EXIT_FAILURE);
  }
}

void capture_init(LocalNode *node) {
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

void capture_start(char *down_iface) {
  // Create the pcap file for the outgoing downed link
  for (int i = 0; i < this->node.n_neighbours; i++) {
    if (strcmp(this->interfaces[i], down_iface)) {
      char filename[24];
      sprintf(filename, "%s.pcap", down_iface);
      dumpers[i] = pcap_dump_open(cap_infos[i].handle, filename);
    }
  }
  // Dispatch all pcap listeners if this is the first downed link
  if (n_down_ifaces == 0) {
    for (int i = 0; i < this->node.n_neighbours; i++) {
      pcap_dispatch(cap_infos[i].handle, -1, dump_packet, NULL);
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

void capture_end(char* up_iface) {
  log_f("ENDED CAPTURE");
  n_down_ifaces--;
  // If all links are now up, stop pcap listeners
  if (n_down_ifaces == 0) {
    for (int i = 0; i < this->node.n_neighbours; i++) {
      pcap_breakloop(cap_infos[i].handle);
    }
  }
  // Close the pcap file for the newly up outgoing interface
  for (int i = 0; i < this->node.n_neighbours; i++) {
    if (strcmp(down_ifaces[i], up_iface)) {
      down_ifaces[i] = NULL;
      pcap_dump_flush(dumpers[i]);
      pcap_dump_close(dumpers[i]);
      break;
    }
  }
}



