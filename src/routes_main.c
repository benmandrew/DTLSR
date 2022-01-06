
#include "filenames.h"
#include "process/route_control.h"


struct rtentry routes[MAX_NODE_NUM];

void populate_routes(LocalNode* this) {
	memset(routes, 0, MAX_NODE_NUM * sizeof(struct rtentry));

	for (int i = 0; i < MAX_NODE_NUM; i++) {
		struct sockaddr_in *addr = (struct sockaddr_in*) &routes[i].rt_gateway;
		addr->sin_family = AF_INET;
		addr->sin_addr.s_addr = this->node.neighbour_ips[i];

		addr = (struct sockaddr_in*) &routes[i].rt_dst;
		addr->sin_family = AF_INET;
		addr->sin_addr.s_addr = this->node.neighbour_ips[i];

		addr = (struct sockaddr_in*) &routes[i].rt_genmask;
		addr->sin_family = AF_INET;
		addr->sin_addr.s_addr = INADDR_ANY;
		
		routes[i].rt_dev = this->interfaces[i];
		routes[i].rt_flags = RTF_UP | RTF_GATEWAY;
		routes[i].rt_metric = 0;
	}
	return routes;
}


int driver(int argc, char** argv) {
	LocalNode this;
	init_this_node(&this, PROTOCOL, CONFIG, HEARTBEAT_TIMEOUT);
	routes = get_routes(&this);






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
