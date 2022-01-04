
#include <string.h>

#include "algorithm/node.h"

Node node_alloc(int n) {
	Node node;
	node.state = NODE_SEEN;
	node.n_neighbours = n;
	node.neighbour_ids = (int*)malloc(n * sizeof(int));
	node.neighbour_ips = (uint32_t*)malloc(n * sizeof(uint32_t));
	node.source_ips = (uint32_t*)malloc(n * sizeof(uint32_t));
	node.neighbour_links_alive = (char*)malloc(n * sizeof(char));
	return node;
};

// 'eth' + max 2 digit ID + null terminator
#define IFACE_STR_MAX_LEN 6

LocalNode node_local_alloc(int n, int hb_timeout) {
	LocalNode node;
	node.node = node_alloc(n);
	node.timers = (Timer*)malloc(n * sizeof(Timer));
	node.interfaces = (char**)malloc(n * sizeof(char*));
	node.if_arena_ptr = (char*)malloc(IFACE_STR_MAX_LEN * n * sizeof(char));
	for (int i = 0; i < n; i++) {
		node.timers[i] = event_timer_append(hb_timeout, 0);
		node.interfaces[i] = node.if_arena_ptr + i * (IFACE_STR_MAX_LEN * sizeof(char));
	}
	return node;
};

void node_dealloc(Node* n) {
	free(n->neighbour_ids);
	free(n->neighbour_ips);
	free(n->source_ips);
	free(n->neighbour_links_alive);
}

void node_local_dealloc(LocalNode* n) {
	node_dealloc(&n->node);
	free(n->timers);
	free(n->interfaces);
	free(n->if_arena_ptr);
}

void node_update_time(Node* n) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	n->timestamp =
		(unsigned long long)(tv.tv_sec) * 1000 +
    (unsigned long long)(tv.tv_usec) / 1000;
}
