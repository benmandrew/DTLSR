
#ifndef GRAPH_PI_H
#define GRAPH_PI_H

#include "algorithm/graph.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "algorithm/node.h"
#include "network/packetsend.h"
#include "process/daemonise.h"
#include "process/fd_event.h"
#include "process/logging.h"

char merge_in(Node *these, Node *others);

char register_heartbeat(LocalNode *this, struct hop_dest *next_hops, long source_addr, struct status *s);

#endif
