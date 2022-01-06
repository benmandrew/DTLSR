
#include <errno.h>

// #include "filenames.h"
#include "process/route_control.h"
#include "process/core_node.h"
#include "algorithm/pathfind.h"

#define PROTOCOL "routes"
#define CONFIG "routes"

void set_addrs(struct rtentry* route, uint32_t gateway_ip, uint32_t dest_ip) {
	struct sockaddr_in *addr = (struct sockaddr_in*) &route->rt_gateway;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = gateway_ip;
	addr = (struct sockaddr_in*) &route->rt_dst;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = dest_ip;
	addr = (struct sockaddr_in*) &route->rt_genmask;
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = inet_addr("255.255.255.0"); // x.x.x.x/24
}

void add_routes(LocalNode* this, struct hop_dest* next_hops) {
	struct rtentry route;
	memset(&route, 0, sizeof route);
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	for (int hop_i = 0; hop_i < MAX_NODE_NUM; hop_i++) {
		if (next_hops[hop_i].next_hop == -1) {
			continue;
		}
		struct in_addr addr;
		addr.s_addr = next_hops[hop_i].dest_ip;
		for (int nb_i = 0; nb_i < this->node.n_neighbours; nb_i++) {
			if (this->node.neighbour_ids[nb_i] == next_hops[hop_i].next_hop) {
				set_addrs(&route, this->node.neighbour_ips[nb_i], next_hops[hop_i].dest_ip);
				route.rt_flags = RTF_UP | RTF_GATEWAY;
				int err;
				if (err = ioctl(fd, SIOCADDRT, &route) < 0) {
					log_f("1. ioctl failed: errno %d %s", err, strerror(errno));
				}
				if (err = ioctl(fd, SIOCADDRT, &route) < 0) {
					log_f("2. ioctl failed: errno %d %s", err, strerror(errno));
				}
				break;
			}
		}
	}
}

// Don't want to add routes to direct neighbours, as these should already exist
void remove_neighbours(LocalNode* this, struct hop_dest* next_hops) {
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		for (int j = 0; j < this->node.n_neighbours; j++) {
			if (this->node.neighbour_ips[j] == next_hops[i].dest_ip) {
				next_hops[i].next_hop = -1;
				next_hops[i].dest_ip = 0;
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

int driver(int argc, char** argv) {
	LocalNode this;
	init_this_node(&this, PROTOCOL, CONFIG, HEARTBEAT_TIMEOUT);

	int id = get_node_id("routes");
	if (id != 2) return 1;

	struct hop_dest next_hops[MAX_NODE_NUM];
	memset(next_hops, 0, sizeof next_hops);
	next_hops[0].next_hop = 1;
	next_hops[0].dest_ip = inet_addr("10.0.0.10");
	next_hops[1].next_hop = -1;
	next_hops[1].dest_ip = inet_addr("0.0.0.0");
	next_hops[2].next_hop = 6;
	next_hops[2].dest_ip = inet_addr("10.0.1.11");
	next_hops[3].next_hop = 6;
	next_hops[3].dest_ip = inet_addr("10.0.1.10");
	next_hops[4].next_hop = -1;
	next_hops[4].dest_ip = inet_addr("0.0.0.0");
	next_hops[5].next_hop = 6;
	next_hops[5].dest_ip = inet_addr("10.0.2.1");
	for (int i = 6; i < MAX_NODE_NUM; i++) next_hops[i].next_hop = -1;
	
	remove_neighbours(&this, next_hops);
	reduce_routes_to_subnets(next_hops);
	add_routes(&this, next_hops);

	// int fd = socket(AF_INET, SOCK_DGRAM, 0);
	// struct rtentry route;
	// memset(&route, 0, sizeof route);
	// struct sockaddr_in *addr = (struct sockaddr_in*) &route.rt_gateway;
	// addr->sin_family = AF_INET;
	// addr->sin_addr.s_addr = inet_addr("10.0.2.1");
	// addr = (struct sockaddr_in*) &route.rt_dst;
	// addr->sin_family = AF_INET;
	// addr->sin_addr.s_addr = inet_addr("10.0.1.0");
	// addr = (struct sockaddr_in*) &route.rt_genmask;
	// addr->sin_family = AF_INET;
	// addr->sin_addr.s_addr = inet_addr("255.255.255.0"); // x.x.x.x/24
	// route.rt_flags = RTF_UP | RTF_GATEWAY;

	// if (ioctl(fd, SIOCADDRT, &route) < 0) {
	// 	log_f("ioctl failed: errno %s", strerror(errno));
	// }

	while (1);

	return 0;
}


int main(int argc, char* argv[]) {
	set_logfile_name("routes");
	log_f("routes started");
	int daemonise = 0;
	int opt;
	while ((opt = getopt(argc, argv, "d")) != -1) {
		switch (opt) {
			case 'd': daemonise = 1; break;
			default:
				log_f("routes usage: %s [-d]", argv[0]);
				exit(EXIT_FAILURE);
		}
	}
	if (daemonise) {
		make_daemon();
		log_f("daemonisation successful");
	}
	event_init();
	return driver(argc, argv);
}
