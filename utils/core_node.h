
#ifndef CORE_NODE_H
#define CORE_NODE_H

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <net/route.h>

#include "node.h"
#include "fileio.h"

int get_node_id(char* protocol);

int get_neighbours(LocalNode* neighbours, char* protocol);

#endif
