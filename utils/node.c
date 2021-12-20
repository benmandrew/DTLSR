
#include "node.h"

Node alloc_node(int n) {
	Node node;
	node.n_neighbours = n;
	node.neighbour_ips = (uint32_t*)malloc(n * sizeof(uint32_t));
	node.source_ips = (uint32_t*)malloc(n * sizeof(uint32_t));
	node.neighbour_links_alive = (char*)malloc(n * sizeof(char));
	return node;
};


LocalNode alloc_local_node(int n) {
	LocalNode node;
	node.node = alloc_node(n);
	node.timers = (Timer*)malloc(n * sizeof(Timer));
	node.interfaces = (char**)malloc(n * sizeof(char*));
	return node;
};
