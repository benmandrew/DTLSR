
#include "process/core_node.h"
#include "process/core_node_pi.h"

#define CONFIG_PATH "/home/ben/projects/dtlsr/configs"

char *read_node_id_str(char *protocol) {
  char filename[FILENAME_MAX];
  sprintf(filename, "%s.id", protocol);
  return read_file(filename);
}

char *read_node_conf_file(char *protocol, char *config) {
  char *node_id = read_node_id_str(protocol);
  // Use node ID to find the corresponding config file
  char filename[FILENAME_MAX];
  sprintf(filename, "%s/%s/n%s", CONFIG_PATH, config, node_id);
  free(node_id);
  return read_file(filename);
}

int parse_link(char *pch, LocalNode *this, int i) {
  // Format:
  // iface - src_addr - dst_addr - dst_id
  char *saved;
  char *interface = strtok_r(pch, " - ", &saved);
  char *src_addr = strtok_r(NULL, " - ", &saved);
  char *dst_addr = strtok_r(NULL, " - ", &saved);
  char *dst_id = strtok_r(NULL, " - ", &saved);

  strcpy(this->interfaces[i], interface);
  inet_pton(AF_INET, src_addr, &(this->node.source_ips[i]));
  inet_pton(AF_INET, dst_addr, &(this->node.neighbour_ips[i]));
  struct in_addr a;
  a.s_addr = this->node.neighbour_ips[i];
  this->node.neighbour_ids[i] = atoi(dst_id);
}

int get_node_id(char *protocol) { return atoi(read_node_id_str(protocol)); }

// Derive the number of neighbours from the neighbour file contents
int parse_n_neighbours(char *contents) {
  int n = 0, i = 0;
  while (contents[i] != '\0') {
    if (contents[i] == '\n')
      n++;
    i++;
  }
  return n;
}

unsigned long long get_now(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (unsigned long long)(tv.tv_sec) * 1000 +
         (unsigned long long)(tv.tv_usec) / 1000;
}

void local_node_init(LocalNode *this, char *protocol, char *config,
                     long hb_timeout) {
  unsigned long long now = get_now();
  char *contents = read_node_conf_file(protocol, config);
  // Get number of neighbours
  int n = parse_n_neighbours(contents);
  int id = get_node_id(protocol);
  *this = node_local_alloc(id, n, hb_timeout);
  // Save start so contents can be freed
  char *start = contents;
  char *saved;
  char *pch = strtok_r(contents, "\n", &saved);
  int i = 0;
  while (pch != NULL) {
    parse_link(pch, this, i);
    pch = strtok_r(NULL, "\n", &saved);
    // Links start DOWN
    this->node.link_statuses[i] = LINK_DOWN;
#ifdef DTLSR
    ts_init(&this->ls_time_series[i], LINK_DOWN, now);
#endif
    event_timer_disarm(&this->timers[i]);
    i++;
  }
  free(start);
}
