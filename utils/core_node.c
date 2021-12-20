
#include "core_node.h"

#define CONFIG_PATH "/home/ben/projects/routing/configs"

/*
 * Format:
 * ID
 * Example:
 * 1
 */
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
	char* saved;
	char* interface = strtok_r(pch, " - ", &saved);
	char* src_addr = strtok_r(NULL, " - ", &saved);
	char* dst_addr = strtok_r(NULL, " - ", &saved);

	strcpy(this->interfaces[i], interface);
	inet_pton(AF_INET, src_addr, &(this->node.source_ips[i]));
	inet_pton(AF_INET, dst_addr, &(this->node.neighbour_ips[i]));
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

/*
 * Format:
 * Interface - Source - Destination
 * Example:
 * eth0 - 10.0.0.2 - 10.0.0.1
 * eth1 - 10.0.1.1 - 10.0.1.2
 */
int get_neighbours(LocalNode* this, char* protocol) {
	this->node.id = get_node_id(protocol);
	char* contents = _read_node_conf_file(protocol);
	// Get number of neighbours
	int n = _parse_n_neighbours(contents);
	*this = alloc_local_node(n);
	// Save start so contents can be freed
	char* start = contents;
	char* saved;
	char* pch = strtok_r(contents, "\n", &saved);
	int i = 0;
	while(pch != NULL) {
		_parse_link(pch, this, i);
		pch = strtok_r(NULL, "\n", &saved);
		i++;
	}
	free(start);
	return n;
}