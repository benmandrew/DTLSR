
#ifndef CORE_NODE_H
#define CORE_NODE_H

#include <arpa/inet.h>
#include <net/route.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "algorithm/node.h"
#include "process/fileio.h"

int get_node_id(char *protocol);

unsigned long long get_now(void);

// Initialise 'this' node, allocating and populating neighbour information
void local_node_init(LocalNode *this, char *protocol, char *config,
                    double hb_timeout);

#endif
