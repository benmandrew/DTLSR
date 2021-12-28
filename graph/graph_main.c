
#include "def.h"
#include "graph.h"

LSSockets init_sockets(void) {
	LSSockets socks;
	socks.hb_sock = get_open_socket(HB_PORT);
	socks.lsa_rec_sock = get_open_socket(LSA_PORT);
	socks.lsa_snd_sock = get_socket();
	event_append(socks.hb_sock);
	event_append(socks.lsa_rec_sock);
	aggregate_fds(&socks, N_EVENT_SOCKS);
	return socks;
}

void close_sockets(LSSockets* socks) {
	close(socks->hb_sock);
	close(socks->lsa_rec_sock);
	close(socks->lsa_snd_sock);
}

int driver(int argc, char** argv) {
	init_this_node(&this, "graph", HEARTBEAT_TIMEOUT);
	LSSockets socks = init_sockets();
	while (1) {
		int active_fd;
		if ((active_fd = event_wait(socks.event_fds, socks.n_event_fds)) < 0) {
			continue;
		}

		if (active_fd == socks.hb_sock) {
			receive_heartbeat(&socks);
		} else if (active_fd == socks.lsa_rec_sock) {
			receive_lsa(&socks);
		} else {
			timeout_heartbeat(active_fd, &socks);
		}
	}
	close_sockets(&socks);
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
