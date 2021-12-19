
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "logging.h"
#include "daemonise.h"
#include "packetsend.h"
#include "fd_event.h"
#include "graph.h"

#define PORT 8080
#define HB_SIZE sizeof(int)
#define HEARTBEAT_T 5
#define HEARTBEAT_TIMEOUT HEARTBEAT_T + 3

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
	struct in_addr a;
	a.s_addr = source_addr;
	log_f("%s UP", inet_ntoa(a));

	for (int i = 0; i < this.n_neighbours; i++) {
		if (this.neighbour_ips[i] == source_addr) {
			this.neighbour_links_alive[i] = 1;
			event_timer_arm(&timers[i], HEARTBEAT_TIMEOUT, 0);
			break;
		}
	}
}

void timeout_heartbeat(int active_fd) {
	for (int i = 0; i < this.n_neighbours; i++) {
		if (timers[i].fd == active_fd) {
			this.neighbour_links_alive[i] = 0;
			struct in_addr a;
			a.s_addr = this.neighbour_ips[i];
			log_f("%s DOWN", inet_ntoa(a));
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

int driver(int argc, char** argv) {
	char buffer[HB_SIZE];
	int sockfd = get_open_socket(PORT);
	event_append(sockfd);
	init_node();
	int* fds = init_fds(sockfd);
	int n_fds = this.n_neighbours + 1;
	int active_fd;
	while (1) {
		if ((active_fd = event_wait(fds, n_fds)) < 0) {
			continue;
		}
		if (active_fd == sockfd) {
			struct sockaddr_in from;
			int n = ps_receive(sockfd, (void*)buffer, HB_SIZE, (struct sockaddr*)&from);
			register_heartbeat((long)from.sin_addr.s_addr);
		} else {
			timeout_heartbeat(active_fd);
		}
	}
	// Close timer sockets pls
	return 0;
}

int main(int argc, char* argv[]) {
	set_logfile_name("graph");
	log_f("Graph Started");
	int daemonise = 0;
	int opt;
	while ((opt = getopt(argc, argv, "d")) != -1) {
		switch (opt) {
			case 'd': daemonise = 1; break;
			default:
				log_f("Server usage: %s [-d]", argv[0]);
				exit(EXIT_FAILURE);
		}
	}
	if (daemonise) {
		make_daemon();
		log_f("Daemonisation successful");
	}
	event_init();
	return driver(argc, argv);
}