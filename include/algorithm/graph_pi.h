
#ifndef GRAPH_PI_H
#define GRAPH_PI_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "algorithm/graph.h"
#include "algorithm/node.h"
#include "network/packetsend.h"
#include "process/daemonise.h"
#include "process/fd_event.h"
#include "process/logging.h"

char merge_in_graph(Node *these, Node *others);

char merge_in_node(Node *graph, Node *other);

char register_heartbeat(LocalNode *this, LSFD *fds, struct hop_dest *next_hops,
                        long source_addr, char **up_iface);

#endif
