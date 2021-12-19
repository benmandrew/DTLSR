
#include "graph.h"

int driver(int argc, char** argv) {
	char buffer[HB_SIZE];
	int sockfd = get_open_socket(PORT);
	event_append(sockfd);
	init_node();
	int* fds = init_fds(sockfd);
	int n_fds = get_n_neighbours() + 1;
	int active_fd;
	while (1) {
		if ((active_fd = event_wait(fds, n_fds)) < 0) {
			continue;
		}
		if (active_fd == sockfd) {
			struct sockaddr_in from;
			int n = ps_receive(sockfd, (void*)buffer, HB_SIZE, (struct sockaddr*)&from);
			register_heartbeat((long)from.sin_addr.s_addr);
		} else {
			timeout_heartbeat(active_fd);
		}
	}
	// Close timer sockets pls
	return 0;
}

int main(int argc, char* argv[]) {
	set_logfile_name("graph");
	log_f("Graph Started");
	int daemonise = 0;
	int opt;
	while ((opt = getopt(argc, argv, "d")) != -1) {
		switch (opt) {
			case 'd': daemonise = 1; break;
			default:
				log_f("Server usage: %s [-d]", argv[0]);
				exit(EXIT_FAILURE);
		}
	}
	if (daemonise) {
		make_daemon();
		log_f("Daemonisation successful");
	}
	event_init();
	return driver(argc, argv);
}
