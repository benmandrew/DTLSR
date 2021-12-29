
#include "process/fd_event.h"

#define DISARM_T __INT64_MAX__

fd_set s;

void event_init(void) {
	FD_ZERO(&s);
}

void event_append(int fd) {
	FD_SET(fd, &s);
}

void event_timer_reset(Timer* t) {
	timerfd_settime(t->fd, 0, &(t->v), NULL);
}

void event_timer_disarm(Timer* t) {
	t->v = (struct itimerspec) {
		.it_interval = {0, 0},
		.it_value    = {DISARM_T, 0},
	};
	event_timer_reset(t);
}

void event_timer_arm(Timer* t, int sec, int nsec) {
	t->v = (struct itimerspec) {
		.it_interval = {0, 0},
		.it_value    = {sec, nsec},
	};
	event_timer_reset(t);
}

Timer event_timer_append(int sec, int nsec) {
	Timer t;
	t.fd = timerfd_create(CLOCK_REALTIME, 0);
	event_timer_arm(&t, sec, nsec);
	event_timer_reset(&t);
	event_append(t.fd);
	return t;
}

int event_wait(int* fds, int n_fds) {
	// 'select' modifies the 'fd_set', so make a copy and pass that
	fd_set copy = s;
	int n_ready;
	if ((n_ready = select(FD_SETSIZE, &copy, NULL, NULL, NULL)) < 0) {
		exit(EXIT_FAILURE);
	} else if (n_ready == 0) {
		return -1;
	}
	for (int i = 0; i < n_fds; i++) {
		if(FD_ISSET(fds[i], &copy)) {
			return fds[i];
		}
	}
	return -1;
}

void event_timer_dealloc(Timer timer) {
	close(timer.fd);
}
