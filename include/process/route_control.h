
#ifndef ROUTE_CONTROL_H
#define ROUTE_CONTROL_H

#include <sys/ioctl.h>

#include "algorithm/graph.h"
#include "algorithm/pathfind.h"

void update_routes(LocalNode* this, struct hop_dest* next_hops);

void log_routes(void);

#endif
