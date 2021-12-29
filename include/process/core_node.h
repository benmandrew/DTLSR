
#ifndef CORE_NODE_H
#define CORE_NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <net/route.h>

#include "graph/node.h"
#include "process/fileio.h"

int get_node_id(char* protocol);

// Initialise 'this' node, allocating and populating neighbour information
void init_this_node(LocalNode* this, char* protocol, int hb_timeout);

#endif
