
#include "def.h"
#include "graph.h"

#define MAX_NODE_NUM 64
#define NODES_SIZE MAX_NODE_NUM * sizeof(Node)
#define LSA_SIZE NODES_SIZE + sizeof(int)

Graph g;

void init_graph(void) {
	g.n_nodes = MAX_NODE_NUM;
	g.nodes = (Node*)malloc(MAX_NODE_NUM * sizeof(Node));
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		// This is how we show a node with id=i doesn't exist
		g.nodes[i].id = -1;
	}
}

void local_update(Node* this) {
	memcpy(&g.nodes[this->id], this, sizeof(Node));

	// Not sure if neighbour back-links should be updated (or even need to be)

	// Update neighbour back-links
	// for (int i = 0; i < this->n_neighbours; i++) {
	// 	int id = this->neighbour_ids[i];
	// 	Node* n = &g.nodes[id];
	// 	n->
	// }
}

char merge_in(Node* nodes) {
	char updated = 0;
	for (int id = 0; id < MAX_NODE_NUM; id++) {
		Node* this = &g.nodes[id];
		Node* other = &nodes[id];
		// Node exists in neither graph, or only exists in our graph
		if (other->id == -1) {
			continue;
		}
		// Other graph has a new node we haven't seen, or it has an earlier timestamp
		if (this->id == -1 || (other->timestamp < this->timestamp)) {
			memcpy(this, other, sizeof(Node));
			updated = 1;
		}
	}
	return updated;
}

char buffer[LSA_SIZE];

void receive_lsa(LSSockets* socks) {
	struct sockaddr_in from;
	receive(socks->lsa_rec_sock, (void*)buffer, LSA_SIZE, (struct sockaddr*)&from);
	char updated = merge_in((Node*)buffer);
	if (updated) {
		int sending_node_id = buffer[NODES_SIZE];
		send_lsa_except(socks, sending_node_id);
	}
}

// Send LSA to all neighbours except one
void send_lsa_except(LSSockets* socks, int received_id) {
	// Store nodes
	memcpy(buffer, g.nodes, NODES_SIZE);
	// Store sending node ID at the end of the buffer
	buffer[NODES_SIZE] = this.node.id;
	struct rtentry* routes = get_routes(&this);
	for (int i = 0; i < this.node.n_neighbours; i++) {
		// Skip neighbour from which LSA was originally received
		if (this.node.neighbour_ids[i] == received_id) continue;
		// Populate address struct
		struct sockaddr_in* neighbour_addr = (struct sockaddr_in*)&(routes[i].rt_dst);
		neighbour_addr->sin_port = htons(LSA_PORT);
		int addr_len = sizeof(*neighbour_addr);
		// Send LSA to neighbour
		sendto(socks->lsa_snd_sock, buffer, LSA_SIZE, MSG_CONFIRM,
			(const struct sockaddr *) neighbour_addr, addr_len);
	}
	free(routes);
}

// Send LSA to all neighbours
void send_lsa(LSSockets* socks) {
	send_lsa_except(socks, -1);
}
