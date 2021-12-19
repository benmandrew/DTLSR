
#include "def.h"
#include "graph.h"

Node this;
Timer* timers;

void init_node(void) {
	this.ip = 0;
	struct rtentry* entries;
	this.n_neighbours = get_neighbours(&entries, "graph");
	this.neighbour_ips = (uint32_t*)malloc(this.n_neighbours * sizeof(uint32_t));
	this.neighbour_links_alive = (char*)malloc(this.n_neighbours * sizeof(char));
	timers = (Timer*)malloc(this.n_neighbours * sizeof(Timer));
	for (int i = 0; i < this.n_neighbours; i++) {
		// Extract destination address
		struct sockaddr_in* addr = (struct sockaddr_in*)&(entries[i].rt_dst);
		this.neighbour_ips[i] = (uint32_t)addr->sin_addr.s_addr;
		// Links start DOWN
		this.neighbour_links_alive[i] = 0;
		timers[i] = event_timer_append(HEARTBEAT_TIMEOUT, 0);
	}
}

char register_heartbeat(long source_addr) {
	char updated = 0;
	for (int i = 0; i < this.n_neighbours; i++) {
		if (this.neighbour_ips[i] == source_addr) {
			// Reset timer
			event_timer_arm(&timers[i], HEARTBEAT_TIMEOUT, 0);
			// If link was DOWN then it is now UP
			if (!this.neighbour_links_alive[i]) {
				log_f("%s UP", ip_to_str(source_addr));
				this.neighbour_links_alive[i] = 1;
				updated = 1;
				break;
			}
		}
	}
	return updated;
}

char timeout_heartbeat(int active_fd) {
	char updated = 0;
	for (int i = 0; i < this.n_neighbours; i++) {
		if (timers[i].fd == active_fd) {
			this.neighbour_links_alive[i] = 0;
			log_f("%s DOWN", ip_to_str(this.neighbour_ips[i]));
			event_timer_disarm(&timers[i]);
			updated = 1;
			break;
		}
	}
	return updated;
}

int* init_fds(int* sockfds, int n_sockfds) {
	int i;
	int* fds = (int*)malloc((this.n_neighbours + n_sockfds) * sizeof(int));
	for (i = 0; i < this.n_neighbours; i++) {
		fds[i] = timers[i].fd;
	}
	for (i = 0; i < n_sockfds; i++) {
		fds[i + this.n_neighbours] = sockfds[i];
	}
	return fds;
}

int get_n_neighbours(void) {
	return this.n_neighbours;
}
