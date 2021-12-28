
#include "core_node.h"

#define CONFIG_PATH "/home/ben/projects/routing/configs"

char* _read_node_id_str(char* protocol) {
	char filename[FILENAME_MAX];
	sprintf(filename, "%s.id", protocol);
	return read_file(filename);
}

char* _read_node_conf_file(char* protocol) {
	char* node_id = _read_node_id_str(protocol);
	// Use node ID to find the corresponding config file
	char filename[FILENAME_MAX];
	sprintf(filename, "%s/%s/n%s", CONFIG_PATH, protocol, node_id);
	free(node_id);
	return read_file(filename);
}

int _parse_link(char* pch, LocalNode* this, int i) {
	// Format:
	// iface - src_addr - dst_addr - dst_id
	char* saved;
	char* interface = strtok_r(pch, " - ", &saved);
	char* src_addr = strtok_r(NULL, " - ", &saved);
	char* dst_addr = strtok_r(NULL, " - ", &saved);
	char* dst_id = strtok_r(NULL, " - ", &saved);

	strcpy(this->interfaces[i], interface);
	inet_pton(AF_INET, src_addr, &(this->node.source_ips[i]));
	inet_pton(AF_INET, dst_addr, &(this->node.neighbour_ips[i]));
	this->node.neighbour_ids[i] = atoi(dst_id);
}

int get_node_id(char* protocol) {
	return atoi(_read_node_id_str(protocol));
}

// Derive the number of neighbours from the neighbour file contents
int _parse_n_neighbours(char* contents) {
	int n = 0, i = 0;
	while (contents[i] != '\0') {
		if (contents[i] == '\n') n++;
		i++;
	}
	return n;
}

void init_this_node(LocalNode* this, char* protocol, int hb_timeout) {
	this->node.id = get_node_id(protocol);
	char* contents = _read_node_conf_file(protocol);
	// Get number of neighbours
	int n = _parse_n_neighbours(contents);
	*this = alloc_local_node(n, hb_timeout);
	// Save start so contents can be freed
	char* start = contents;
	char* saved;
	char* pch = strtok_r(contents, "\n", &saved);
	int i = 0;
	while(pch != NULL) {
		_parse_link(pch, this, i);
		pch = strtok_r(NULL, "\n", &saved);
		// Links start DOWN
		this->node.neighbour_links_alive[i] = 0;
		event_timer_disarm(&this->timers[i]);
		i++;
	}
	free(start);
}
