
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
		// Links start down
		this.neighbour_links_alive[i] = 0;
		// Add a bit of fudge factor
		timers[i] = event_timer_append(HEARTBEAT_TIMEOUT, 0);
	}
}

void register_heartbeat(long source_addr) {
	for (int i = 0; i < this.n_neighbours; i++) {
		if (this.neighbour_ips[i] == source_addr) {
			// Reset timer
			event_timer_arm(&timers[i], HEARTBEAT_TIMEOUT, 0);
			// If link was DOWN then it is now UP
			if (!this.neighbour_links_alive[i]) {
				log_f("%s UP", ip_to_str(source_addr));
				this.neighbour_links_alive[i] = 1;
				break;
			}
		}
	}
}

void timeout_heartbeat(int active_fd) {
	for (int i = 0; i < this.n_neighbours; i++) {
		if (timers[i].fd == active_fd) {
			this.neighbour_links_alive[i] = 0;
			log_f("%s DOWN", ip_to_str(this.neighbour_ips[i]));
			event_timer_disarm(&timers[i]);
			break;
		}
	}
}

int* init_fds(int sockfd) {
	int* fds = (int*)malloc((this.n_neighbours + 1) * sizeof(int));
	memcpy(fds, timers, this.n_neighbours * sizeof(int));
	fds[this.n_neighbours] = sockfd;
	return fds;
}

int get_n_neighbours(void) {
	return this.n_neighbours;
}
