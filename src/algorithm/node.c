
#include <string.h>

#include "algorithm/node.h"

void node_init(Node* node, int n) {
	if (n > MAX_NODE_FAN) abort();
	node->state = NODE_SEEN;
	node->n_neighbours = n;
	node->timestamp = 0;
};

// 'eth' + max 2 digit ID + null terminator
#define IFACE_STR_MAX_LEN 6

LocalNode node_local_alloc(int id, int n, int hb_timeout) {
	LocalNode node;
	node_init(&node.node, n);
	node.node.id = id;
	node.timers = (Timer*)malloc(n * sizeof(Timer));
	node.interfaces = (char**)malloc(n * sizeof(char*));
	node.if_arena_ptr = (char*)malloc(IFACE_STR_MAX_LEN * n * sizeof(char));
	for (int i = 0; i < n; i++) {
		node.timers[i] = event_timer_append(hb_timeout, 0);
		node.interfaces[i] = node.if_arena_ptr + i * (IFACE_STR_MAX_LEN * sizeof(char));
	}
	return node;
};

void node_local_dealloc(LocalNode* n) {
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
