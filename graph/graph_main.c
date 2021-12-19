
#include "def.h"
#include "graph.h"

char buffer[HB_SIZE];

char receive_heartbeat(int hb_sockfd) {
	struct sockaddr_in from;
	ps_receive(hb_sockfd, (void*)buffer, HB_SIZE, (struct sockaddr*)&from);
	return register_heartbeat((long)from.sin_addr.s_addr);
}

int driver(int argc, char** argv) {
	char graph_updated = 0;
	int hb_sockfd = get_open_socket(HB_PORT);
	event_append(hb_sockfd);
	init_node();
	int* fds = init_fds(&hb_sockfd, 1);
	int n_fds = get_n_neighbours() + 1;
	int active_fd;
	while (1) {
		if ((active_fd = event_wait(fds, n_fds)) < 0) {
			continue;
		}
		if (active_fd == hb_sockfd) {
			graph_updated = receive_heartbeat(active_fd);
		} else {
			graph_updated = timeout_heartbeat(active_fd);
		}

		if (graph_updated) {
			graph_updated = 0;
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
