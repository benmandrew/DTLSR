// https://www.geeksforgeeks.org/udp-server-client-implementation-c/

// Client side implementation of UDP client-server model
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
#include "fd_event.h"
#include "packetsend.h"

#define PORT 8080
#define MAXLINE 1024

LocalNode this;

// Driver code
int driver(int argc, char** argv) {
	int sockfd;
	char buffer[MAXLINE];
	char *hello = "Hello from client";
	// struct sockaddr_in servaddr, myaddr;

	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		log_f("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	int n = get_neighbours(&this, "hello");
	struct rtentry* routes = get_routes(&this);


	struct sockaddr_in* servaddr = (struct sockaddr_in*)&(routes[0].rt_dst);
	
	servaddr->sin_port = htons(PORT);
	
	event_init();
	Timer timer = event_timer_append(2, 0);

	while (1) {
		if (event_wait(&timer.fd, 1) < 0) {
			continue;
		}
		event_timer_reset(&timer);
		// sleep(3);
		int addr_len = sizeof(*servaddr);
		sendto(sockfd, (const char *)hello, strlen(hello),
			MSG_CONFIRM, (const struct sockaddr *) servaddr, addr_len);
		log_f("Hello message sent.");
			
		int n = recvfrom(sockfd, (char *)buffer, MAXLINE,
					MSG_WAITALL, (struct sockaddr *) servaddr,
					&addr_len);
		buffer[n] = '\0';
		log_f("Server : %s", buffer);
	}
	close(sockfd);
	close(timer.fd);
	return 0;
}

int main(int argc, char* argv[]) {
	set_logfile_name("client");
	log_f("Client started");

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
