
#ifndef ROUTE_CONTROL_PI_H
#define ROUTE_CONTROL_PI_H

#include "process/route_control.h"

#include <sys/ioctl.h>

#include "algorithm/graph.h"
#include "algorithm/pathfind.h"

struct simple_rt {
  uint32_t dst;
  uint32_t gateway;
  short metric;
  char seen;
  char exists;
};

int ioctl_fd;
struct simple_rt curr_routes[MAX_NODE_NUM];

struct simple_rt get_simple_rt(struct rtentry *rt);

char simple_rt_eq(struct simple_rt *rt1, struct simple_rt *rt2);

char simple_rt_addr_eq(struct simple_rt *rt1, struct simple_rt *rt2);

void mark_routes_unseen(void);

void add_to_curr_routes(struct simple_rt *rt);

void mark_route_seen(struct rtentry *rt);

void set_addrs(struct rtentry *route, uint32_t gateway_ip, uint32_t dest_ip);

void remove_marked_routes(void);

void derive_rtentries(LocalNode *this, struct hop_dest *next_hops,
                      struct rtentry *routes);

void add_routes(LocalNode *this, struct hop_dest *next_hops,
                struct rtentry *routes);

void remove_neighbours(LocalNode *this, struct hop_dest *next_hops);

void remove_duplicates(struct hop_dest *next_hops);

void reduce_routes_to_subnets(struct hop_dest *next_hops);

#endif
