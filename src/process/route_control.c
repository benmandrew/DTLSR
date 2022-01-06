
#include "process/route_control.h"

int current_next_hops[MAX_NODE_NUM];

char next_hops_changed(int* new_next_hops) {
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		if (current_next_hops[i] != new_next_hops[i]) return 1;
	}
	return 0;
}

void populate_routes(void) {

}

void update_routes(int* new_next_hops, Node* graph) {
	if (next_hops_changed(new_next_hops)) return;



}







