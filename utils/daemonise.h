
#ifndef DAEMONISE_H
#define DAEMONISE_H

#include <stdlib.h>
#include <unistd.h>

void make_daemon(void) {
	pid_t process_id = 0;
	pid_t sid = 0;
	process_id = fork();
	if (process_id < 0) {
		exit(EXIT_FAILURE);
	}
	// Parent process
	if (process_id > 0) {
		exit(EXIT_SUCCESS);
	}
	sid = setsid();
	if (sid < 0) {
		exit(EXIT_FAILURE);
	}
  // chdir stops it from working for some reason
	// chdir("/");
	// close(STDIN_FILENO);
	// close(STDOUT_FILENO);
	// close(STDERR_FILENO);
}

#endif
