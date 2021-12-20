
#include "packetsend.h"

int get_open_socket(int port) {
	int fd;
	// Creating socket file descriptor
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		log_f("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	// Accept on any incoming
	struct sockaddr_in anyaddr;
	memset(&anyaddr, 0, sizeof(anyaddr));
	anyaddr.sin_family = AF_INET;
	anyaddr.sin_addr.s_addr = INADDR_ANY;
	anyaddr.sin_port = htons(port);
	if (bind(fd, (struct sockaddr*)&anyaddr, sizeof(anyaddr)) < 0) {
		log_f("Bind failed");
		exit(EXIT_FAILURE);
	}
	int on = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const void*)&on, sizeof(on));
	return fd;
}

int get_socket(void) {
	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		log_f("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	return sockfd;
}

int receive(int sockfd, void* buffer, size_t n, struct sockaddr* from) {
	int len = sizeof(struct sockaddr);
	return recvfrom(sockfd, buffer, n, MSG_WAITALL, from, &len);
}

char* ip_to_str(long ip) {
	struct in_addr a;
	a.s_addr = ip;
	return inet_ntoa(a);
}

struct rtentry* get_routes(LocalNode* this) {
	int n = this->node.n_neighbours;
	struct rtentry* routes = (struct rtentry*)
		malloc(n * sizeof(struct rtentry));
	memset(routes, 0, n * sizeof(struct rtentry));

	for (int i = 0; i < n; i++) {
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
