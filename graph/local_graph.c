
#include "def.h"
#include "graph.h"

char register_heartbeat(long source_addr) {
	char updated = 0;
	for (int i = 0; i < this.node.n_neighbours; i++) {
		if (this.node.neighbour_ips[i] == source_addr) {
			// Reset timer
			event_timer_arm(&this.timers[i], HEARTBEAT_TIMEOUT, 0);
			// If link was DOWN then it is now UP
			if (!this.node.neighbour_links_alive[i]) {
				log_f("%s UP", ip_to_str(source_addr));
				this.node.neighbour_links_alive[i] = 1;
				updated = 1;
				break;
			}
		}
	}
	return updated;
}

char buffer[HB_SIZE];

void receive_heartbeat(LSSockets* socks) {
	struct sockaddr_in from;
	receive(socks->hb_sock, (void*)buffer, HB_SIZE, (struct sockaddr*)&from);
	char updated = register_heartbeat((long)from.sin_addr.s_addr);
	if (updated) {
		send_lsa(socks);
	}
}

void timeout_heartbeat(int active_fd, LSSockets* socks) {
	char updated = 0;
	for (int i = 0; i < this.node.n_neighbours; i++) {
		if (this.timers[i].fd == active_fd) {
			this.node.neighbour_links_alive[i] = 0;
			log_f("%s DOWN", ip_to_str(this.node.neighbour_ips[i]));
			event_timer_disarm(&this.timers[i]);
			updated = 1;
			break;
		}
	}
	if (updated) {
		send_lsa(socks);
	}
}

int* init_fds(int* sockfds, int n_sockfds) {
	int i;
	int* fds = (int*)malloc((this.node.n_neighbours + n_sockfds) * sizeof(int));
	for (i = 0; i < this.node.n_neighbours; i++) {
		fds[i] = this.timers[i].fd;
	}
	for (i = 0; i < n_sockfds; i++) {
		fds[i + this.node.n_neighbours] = sockfds[i];
	}
	return fds;
}
