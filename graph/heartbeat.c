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
#include "event.h"
#include "packetsend.h"

#define PORT 8080
#define HEARTBEAT_T 2

struct rtentry* neighbours;

// Driver code
int driver(int argc, char** argv) {
	int sockfd;
	char buffer[MAXLINE];
	char *hello = "Hello from client";

	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		log_f("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	int n = get_neighbours(&neighbours, "graph_update");

	event_init();
	int timer = timer_add(HEARTBEAT_T, 0);

	while (1) {
		if (timer_wait(timer) < 0) {
			continue;
		}
		timer_reset(timer);

		for (int i = 0; i < n; i++) {
			struct sockaddr_in* servaddr = (struct sockaddr_in*)&(neighbours[i].rt_dst);
			servaddr->sin_port = htons(PORT);
			int addr_len = sizeof(*servaddr);
			sendto(sockfd, (char)1, sizeof(char), MSG_CONFIRM,
				(const struct sockaddr *) servaddr, addr_len);
		}
		log_f("Heartbeat sent.");
	}
	close(sockfd);
	timer_dealloc(timer);
	return 0;
}

int main(int argc, char* argv[]) {
	set_logfile_name("heartbeat");
	log_f("Heartbeat started");
	int daemonise = 0;
	int opt;
	while ((opt = getopt(argc, argv, "d")) != -1) {
		switch (opt) {
			case 'd': daemonise = 1; break;
			default:
				fprintf(stderr, "Usage: %s [-d]", argv[0]);
				exit(EXIT_FAILURE);
		}
	}
	if (daemonise) {
		make_daemon();
		log_f("Daemonisation successful");
	}
	return driver(argc, argv);
}