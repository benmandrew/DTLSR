
#ifndef CORE_NODE_PI_H
#define CORE_NODE_PI_H

#include "process/core_node.h"

#include <arpa/inet.h>
#include <net/route.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "algorithm/node.h"
#include "process/fileio.h"

char *read_node_id_str(char *protocol);

char *read_node_conf_file(char *protocol, char *config);

int parse_link(char *pch, LocalNode *this, int i);

int parse_n_neighbours(char *contents);

#endif
