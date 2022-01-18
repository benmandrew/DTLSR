
#ifndef ROUTE_CONTROL_H
#define ROUTE_CONTROL_H

#include <sys/ioctl.h>

#include "algorithm/graph.h"
#include "algorithm/pathfind.h"

void update_routing_table(LocalNode *this, struct hop_dest *next_hops);

void set_gateway_route(struct rtentry *route, uint32_t gateway_ip);

void log_routes(void);

#endif
