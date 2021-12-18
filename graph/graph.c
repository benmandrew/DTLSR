
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
#include "graph.h"

#define PORT 8080
#define MAXLINE 1024

int driver(int argc, char** argv) {
	char buffer[MAXLINE];
	char *hello = "Hello from server";
	
	log_f("Server initialising");
	
	ps_socket s = get_open_socket(PORT);

	while (1) {
		int n = ps_receive(s, buffer);
		buffer[n] = '\0';
		log_f("Client : %s", buffer);
		ps_send(s, hello, strlen(hello));
		log_f("Hello message sent.");
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
	return driver(argc, argv);
}