
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
#include "event.h"
#include "graph.h"

#define PORT 8080
#define HB_SIZE sizeof(int)

int driver(int argc, char** argv) {
	char buffer[HB_SIZE];
	int sockfd = get_open_socket(PORT);
	event_append(sockfd);
	int active_fd;
	while (1) {
		if ((active_fd = event_wait(&sockfd, 1)) < 0) {
			continue;
		}
		if (active_fd == sockfd) {
			struct sockaddr_in from;
			int n = ps_receive(sockfd, (void*)buffer, HB_SIZE, (struct sockaddr*)&from);
			log_f("Heartbeat from %s", inet_ntoa(from.sin_addr));
		}
	}
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