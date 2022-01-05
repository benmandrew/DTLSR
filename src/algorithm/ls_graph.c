
#include <errno.h>

#include "algorithm/def.h"
#include "algorithm/graph.h"
#include "algorithm/pathfind.h"

#define LSA_SIZE MAX_NODE_NUM * sizeof(Node)

void graph_init(Graph* g) {
	g->n_nodes = MAX_NODE_NUM;
	g->nodes = (Node*)malloc(MAX_NODE_NUM * sizeof(Node));
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		g->nodes[i].id = i + 1;
		g->nodes[i].state = NODE_UNSEEN;
	}
}

void graph_dealloc(Graph* g) {
	free(g->nodes);
}

void update_global_this(Graph* g, Node* this) {
	memcpy(&g->nodes[this->id - 1], this, sizeof(Node));

	for (int i = 0; i < this->n_neighbours; i++) {
		int id = this->neighbour_ids[i];
		Node* n = &g->nodes[id - 1];
		if (n->state == NODE_UNSEEN) {
			n->id = id;
			n->state = NODE_OPAQUE;
			n->timestamp = this->timestamp;
		}
	}
}

char merge_in(Node* these, Node* others) {
	char updated = 0;
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		Node* this = &these[i];
		Node* other = &others[i];
		// Node exists in neither graph, or only exists in our graph,
		if (other->state == NODE_UNSEEN) {
			continue;
		}
		// other knows of a node that we have never encountered
		// other state is either opaque or seen
		if (this->state == NODE_UNSEEN) {
			memcpy(this, other, sizeof(Node));
			updated = 1;
		}
		// other is a seen node that we only know of opaquely
		if (this->state == NODE_OPAQUE && other->state == NODE_SEEN) {
			memcpy(this, other, sizeof(Node));
			updated = 1;
		}
		// other is a seen node we have also seen, but a more recent version
		if (other->state == NODE_SEEN || (other->timestamp > this->timestamp)) {
			memcpy(this, other, sizeof(Node));
			updated = 1;
		}
	}
	return updated;
}

char buffer[LSA_SIZE];

void receive_lsa(Graph* g, LocalNode* this, LSSockets* socks) {
	struct sockaddr_in from;
	receive(socks->lsa_rec_sock, (void*)buffer, LSA_SIZE, (struct sockaddr*)&from);
	char updated = merge_in(g->nodes, (Node*)buffer);
	if (updated) {
		send_lsa_except(g, this, socks, (long)from.sin_addr.s_addr);
		log_f("Bruh");
		pathfind_f(g, this->node.id);
		log_f("DONE!!!");
	}
}

// Send LSA to all neighbours except one
void send_lsa_except(Graph* g, LocalNode* this, LSSockets* socks, long source_addr) {
	for (int i = 0; i < this->node.n_neighbours; i++) {
		// Skip neighbour from which LSA was originally received
		if (this->node.neighbour_ips[i] == source_addr) continue;
		// Populate address struct
		struct sockaddr_in* neighbour_addr = (struct sockaddr_in*)&(routes[i].rt_dst);
		neighbour_addr->sin_port = htons(LSA_PORT);
		int addr_len = sizeof(*neighbour_addr);
		// Send LSA to neighbour
		sendto(socks->lsa_snd_sock, (const void*)g->nodes, LSA_SIZE, MSG_CONFIRM,
			(const struct sockaddr*)neighbour_addr, addr_len);
	}
}

// Send LSA to all neighbours
void send_lsa(Graph* g, LocalNode* this, LSSockets* socks) {
	send_lsa_except(g, this, socks, 0);
}
