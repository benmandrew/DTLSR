
#include "def.h"
#include "graph.h"

#define N_SOCKS 2

char buffer[HB_SIZE];

char receive_heartbeat(int hb_sockfd) {
	struct sockaddr_in from;
	receive(hb_sockfd, (void*)buffer, HB_SIZE, (struct sockaddr*)&from);
	return register_heartbeat((long)from.sin_addr.s_addr);
}

void inner_loop(int* fds, int n_fds, int hb_sock, int ls_sock) {
	int active_fd;
	char graph_updated = 0;
	if ((active_fd = event_wait(fds, n_fds)) < 0)
		return;

	if (active_fd == hb_sock)
		graph_updated = receive_heartbeat(active_fd);
	else if (active_fd == ls_sock)
		graph_updated = receive_lsa(active_fd);
	else
		graph_updated = timeout_heartbeat(active_fd);

	if (graph_updated) {
		graph_updated = 0;
	}
}

int driver(int argc, char** argv) {
	int socks[N_SOCKS];
	int hb_sock = get_open_socket(HB_PORT);
	int ls_sock = get_open_socket(LS_PORT);
	socks[0] = hb_sock;
	socks[1] = ls_sock;

	event_append(hb_sock);
	int* fds = init_fds(socks, N_SOCKS);
	int n_fds = get_n_neighbours() + N_SOCKS;
	while (1) {
		inner_loop(fds, n_fds, hb_sock, ls_sock);
	}
	// Close timer sockets pls
	close(hb_sock);
	close(ls_sock);
	return 0;
}

int main(int argc, char* argv[]) {
	set_logfile_name("graph");
	log_f("graph started");
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
		log_f("daemonisation successful");
	}
	event_init();
	return driver(argc, argv);
}
