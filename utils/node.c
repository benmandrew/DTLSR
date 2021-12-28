
#include "node.h"
#include "string.h"

Node alloc_node(int n) {
	Node node;
	node.n_neighbours = n;
	node.neighbour_ids = (int*)malloc(n * sizeof(int));
	node.neighbour_ips = (uint32_t*)malloc(n * sizeof(uint32_t));
	node.source_ips = (uint32_t*)malloc(n * sizeof(uint32_t));
	node.neighbour_links_alive = (char*)malloc(n * sizeof(char));
	return node;
};

// 'eth' + max 2 digit ID + null terminator
#define IFACE_STR_MAX_LEN 6

LocalNode alloc_local_node(int n, int hb_timeout) {
	LocalNode node;
	node.node = alloc_node(n);
	node.timers = (Timer*)malloc(n * sizeof(Timer));
	node.interfaces = (char**)malloc(n * sizeof(char*));
	node.if_arena_ptr = (char*)malloc(IFACE_STR_MAX_LEN * n * sizeof(char));
	for (int i = 0; i < n; i++) {
		node.timers[i] = event_timer_append(hb_timeout, 0);
		node.interfaces[i] = node.if_arena_ptr + i * (IFACE_STR_MAX_LEN * sizeof(char));
	}
	return node;
};

void dealloc_node(Node* n) {
	free(n->neighbour_ids);
	free(n->neighbour_ips);
	free(n->source_ips);
	free(n->neighbour_links_alive);
}

void dealloc_local_node(LocalNode* n) {
	dealloc_node(&n->node);
	free(n->timers);
	free(n->interfaces);
	free(n->if_arena_ptr);
}
