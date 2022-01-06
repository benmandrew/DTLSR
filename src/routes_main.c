
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
		addr->sin_addr.s_addr = inet_addr("255.255.255.0");;
}

void add_routes(LocalNode* this, struct hop_dest* next_hops) {
	struct rtentry route;
	memset(&route, 0, sizeof route);
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	log_f("socket");
	for (int i = 0; i < MAX_NODE_NUM; i++) {
		if (next_hops[i].next_hop == -1) {
			continue;
		}
		struct in_addr addr;
		addr.s_addr = next_hops[i].dest_ip;
		for (int j = 0; j < this->node.n_neighbours; j++) {
			log_f("neighbour id %d", this->node.neighbour_ids[j]);
			if (this->node.neighbour_ids[j] == next_hops[i].next_hop) {
				set_addrs(&route, this->node.neighbour_ips[i], next_hops[i].dest_ip);
				// route.rt_dev = this->interfaces[i];
				route.rt_flags = RTF_UP | RTF_HOST;
				route.rt_metric = 0;
				log_f("set next %d at %s", next_hops[i].next_hop, inet_ntoa(addr));
				if (ioctl(fd, SIOCADDRT, &route) < 0) {
					log_f("ioctl failed and returned errno %s", strerror(errno));
				}
				break;
			}
		}
	}
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
	
	
	add_routes(&this, next_hops);

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
