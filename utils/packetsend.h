
#ifndef PACKETSEND_H
#define PACKETSEND_H

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <net/route.h>

#include "logging.h"
#include "fileio.h"

#define MAXLINE 1024
#define CONFIG_PATH "/home/ben/projects/routing/configs"

typedef struct ps_socket {
	int fd;
	struct sockaddr_in addr;
	socklen_t len;
} ps_socket;

char* _read_node_conf_file(char* protocol) {
	// Get node ID
	char filename[FILENAME_MAX];
	sprintf(filename, "%s.id", protocol);
	char* node_id = read_file(filename);
	// Use node ID to find the corresponding config file
	sprintf(filename, "%s/%s/n%s", CONFIG_PATH, protocol, node_id);
	free(node_id);
	return read_file(filename);
}

int _parse_neighbour(char* pch, struct rtentry* entry) {
	char* saved;
	char* interface = strtok_r(pch, " - ", &saved);
	char* address = strtok_r(NULL, " - ", &saved);

	struct rtentry route;
	memset(&route, 0, sizeof(route));

	struct sockaddr_in *addr = (struct sockaddr_in*) &route.rt_gateway;
	addr->sin_family = AF_INET;
	inet_pton(AF_INET, address, &(addr->sin_addr));

	addr = (struct sockaddr_in*) &route.rt_dst;
	addr->sin_family = AF_INET;
	inet_pton(AF_INET, address, &(addr->sin_addr));

	addr = (struct sockaddr_in*) &route.rt_genmask;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = INADDR_ANY;
	
	route.rt_dev = interface;
	route.rt_flags = RTF_UP | RTF_GATEWAY;
	route.rt_metric = 0;

	*entry = route;
}

/*
 * Parsing example:
 * eth0 - 10.0.0.1
 * eth1 - 10.0.1.2
 */
int get_neighbours(struct rtentry** entries, char* protocol) {
	char* contents = _read_node_conf_file(protocol);
	// Get number of neighbours
	int n = 0, i = 0;
	while (contents[i] != '\0') {
		if (contents[i] == '\n') n++;
		i++;
	}
	*entries = (struct rtentry*)malloc(n * sizeof(struct rtentry));
	// Save start so contents can be freed
	char* start = contents;
	char* saved;
	char* pch = strtok_r(contents, "\n", &saved);
	i = 0;
	while(pch != NULL) {
		_parse_neighbour(pch, *entries + i);
		pch = strtok_r(NULL, "\n", &saved);
		i++;
	}
	free(start);
	return n;
}

// void neighbours_log(struct rtentry* entries, int n) {
// 	for (int i = 0; i < n; i++) {
// 		struct sockaddr_in* addr = (struct sockaddr_in*)&(entries[i].rt_dst);
// 		log_f("N%d: fam=%hu, ip=%s", i+1, addr->sin_family, inet_ntoa(addr->sin_addr));
// 	}
// }

ps_socket get_open_socket(int port) {
	ps_socket s;
	// Creating socket file descriptor
	if ((s.fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		log_f("Socket creation failed");
		exit(EXIT_FAILURE);
	}

	// Accept on any incoming
	struct sockaddr_in anyaddr;
	memset(&anyaddr, 0, sizeof(anyaddr));
	anyaddr.sin_family = AF_INET;
	anyaddr.sin_addr.s_addr = INADDR_ANY;
	anyaddr.sin_port = htons(port);
	if (bind(s.fd, (struct sockaddr*)&anyaddr, sizeof(anyaddr)) < 0) {
		log_f("Bind failed");
		exit(EXIT_FAILURE);
	}
	setsockopt(s.fd, SOL_SOCKET, SO_REUSEADDR, (const void*)1, sizeof(int));
	s.addr = anyaddr;
	s.len = sizeof(anyaddr);
	return s;
}

int ps_receive(ps_socket s, char* buffer) {
	return recvfrom(s.fd, (char *)buffer, MAXLINE, MSG_WAITALL,
									(struct sockaddr *) &s.addr, &s.len);
}

void ps_send(ps_socket s, const void* msg, size_t len) {
	sendto(s.fd, msg, len, MSG_CONFIRM,
				(const struct sockaddr *) &s.addr, s.len);
}

#endif
