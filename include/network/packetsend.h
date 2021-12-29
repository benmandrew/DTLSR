
#ifndef PACKETSEND_H
#define PACKETSEND_H

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <net/route.h>

#include "process/logging.h"
#include "process/fileio.h"
#include "process/core_node.h"

int get_open_socket(int port);

int get_socket(void);

int receive(int sockfd, void* buffer, size_t n, struct sockaddr* from);

char* ip_to_str(long ip);

struct rtentry* get_routes(LocalNode* this);

#endif
