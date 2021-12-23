
#include "def.h"
#include "graph.h"

#define MAX_NODE_NUM 64
#define LS_SIZE MAX_NODE_NUM * sizeof(Node)

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

char buffer[LS_SIZE];

char receive_lsa(int ls_sockfd) {
	struct sockaddr_in from;
	receive(ls_sockfd, (void*)buffer, LS_SIZE, (struct sockaddr*)&from);
	return merge_in((Node*)buffer);
}

void send_lsa() {
	
}





