
#ifndef PACKETSEND_H
#define PACKETSEND_H

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <net/route.h>

#include "logging.h"
#include "fileio.h"

typedef struct ps_socket {
	int fd;
	struct sockaddr_in addr;
	socklen_t len;
} ps_socket;

/*
 * Parsing example:
 * eth0 - 10.0.0.1
 * eth1 - 10.0.1.2
 */
int get_neighbours(struct rtentry** entries, char* protocol);

int get_open_socket(int port);

int get_socket(void);

int ps_receive(int sockfd, void* buffer, size_t n, struct sockaddr* from);

char* ip_to_str(long ip);

#endif
