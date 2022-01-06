
#include <errno.h>

#include "process/route_control.h"

struct simple_rt {
	uint32_t dst;
	uint32_t gateway;
	char seen;
	char exists;
};

struct simple_rt curr_routes[MAX_NODE_NUM];

struct simple_rt get_simple_rt(struct rtentry* rt) {
	struct simple_rt simple;
	struct sockaddr_in* addr = (struct sockaddr_in*) &rt->rt_dst;
	simple.dst = addr->sin_addr.s_addr;
	addr = (struct sockaddr_in*)&rt->rt_gateway;
	simple.gateway = addr->sin_addr.s_addr;
	simple.exists = 1;
	return simple;
}

char simple_rt_eq(struct simple_rt* rt1, struct simple_rt* rt2) {
	return (rt1->dst == rt2->dst &&
					rt1->gateway == rt2->gateway);
}

void mark_routes_unseen(void) {
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		curr_routes[i].seen = 0;
	}
}

void add_to_curr_routes(struct simple_rt* rt) {
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		if (!curr_routes[i].exists) {
			memcpy(&curr_routes[i], rt, sizeof rt);
			return;
		}
	}
}

// Mark route seen if it is, and if it isn't add it to curr_routes
void mark_route_seen(struct rtentry* rt) {
	struct simple_rt simple = get_simple_rt(rt);
	char seen = 0;
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		if (curr_routes[i].exists && simple_rt_eq(&curr_routes[i], &simple)) {
			curr_routes[i].seen = 1;
			seen = 1;
			break;
		}
	}
	if (seen) {
		add_to_curr_routes(&simple);
	}
}

void set_addrs(struct rtentry* route, uint32_t gateway_ip, uint32_t dest_ip) {
	struct sockaddr_in* addr = (struct sockaddr_in*)&route->rt_gateway;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = gateway_ip;
	addr = (struct sockaddr_in*) &route->rt_dst;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = dest_ip;
	addr = (struct sockaddr_in*) &route->rt_genmask;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = inet_addr("255.255.255.0"); // x.x.x.x/24
}

void remove_marked_routes(int fd) {
	mark_routes_unseen();
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		if (curr_routes[i].exists && !curr_routes[i].seen) {
			struct rtentry rt;
			set_addrs(&rt, curr_routes[i].gateway, curr_routes[i].dst);
			if (ioctl(fd, SIOCDELRT, &rt) < 0) {
				log_f("ioctl failed: errno %s", strerror(errno));
			}
			memset(&curr_routes[i], 0, sizeof(struct simple_rt));
		}
	}
}

void derive_rtentries(LocalNode* this, struct hop_dest* next_hops, struct rtentry* routes) {
	for (int hop_i = 0; hop_i < MAX_NODE_NUM; hop_i++) {
		if (next_hops[hop_i].next_hop == -1) {
			continue;
		}
		struct in_addr addr;
		addr.s_addr = next_hops[hop_i].dest_ip;
		for (int nb_i = 0; nb_i < this->node.n_neighbours; nb_i++) {
			if (this->node.neighbour_ids[nb_i] == next_hops[hop_i].next_hop) {
				set_addrs(&routes[hop_i], this->node.neighbour_ips[nb_i], next_hops[hop_i].dest_ip);
				routes[hop_i].rt_flags = RTF_UP | RTF_GATEWAY;
				break;
			}
		}
	}
}

void add_routes(LocalNode* this, struct hop_dest* next_hops, struct rtentry* routes) {
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		if (ioctl(fd, SIOCADDRT, &routes[i]) < 0) {
			log_f("ioctl failed: errno %s", strerror(errno));
		}
	}
}

// Don't want to add routes to direct neighbours, as these should already exist
void remove_neighbours(LocalNode* this, struct hop_dest* next_hops) {
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		for (int j = 0; j < this->node.n_neighbours; j++) {
			if (this->node.neighbour_ips[j] == next_hops[i].dest_ip) {
				next_hops[i].next_hop = -1;
				break;
			}
		}
	}
}

void remove_duplicates(struct hop_dest* next_hops) {
	for (int i = 1; i < MAX_NODE_NUM; i++) {
		for (int j = 0; j < i; j++) {
			if (next_hops[i].next_hop == next_hops[j].next_hop &&
					next_hops[i].dest_ip == next_hops[j].dest_ip) {
				next_hops[i].next_hop = -1;
				break;
			}
		}
	}
}

void reduce_routes_to_subnets(struct hop_dest* next_hops) {
	in_addr_t mask = inet_addr("255.255.255.0");
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		if (next_hops[i].next_hop != -1) {
			next_hops[i].dest_ip &= mask;
		}
	}
	remove_duplicates(next_hops);
}

void update_routes(LocalNode* this, struct hop_dest* next_hops) {
	struct rtentry routes[MAX_NODE_NUM];
	derive_rtentries(this, next_hops, routes);
	remove_neighbours(this, next_hops);
	reduce_routes_to_subnets(next_hops);
	add_routes(this, next_hops, routes);

	mark_routes_unseen();
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		if (next_hops[i].next_hop != -1) {
			mark_route_seen(&routes[i]);
		}
	}
}

void log_routes(void) {
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		if (curr_routes[i].exists) {
			log_f("route %d: %d %d", i+1, curr_routes[i].dst, curr_routes[i].gateway);
		}
	}
}


