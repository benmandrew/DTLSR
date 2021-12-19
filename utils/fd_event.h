
#ifndef FD_EVENT_H
#define FD_EVENT_H

#include <sys/types.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "logging.h"

#define DISARM_T __INT64_MAX__

fd_set s;

typedef struct Timer {
	int fd;
	struct itimerspec v;
} Timer;

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
	// 'select' modifies the 'fd_set'
	fd_set copy = s;
	if (select(FD_SETSIZE, &copy, NULL, NULL, NULL) < 0) {
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < n_fds; i++) {
		log_f("%d %d", fds[i], FD_ISSET(fds[i], &s));
		if(FD_ISSET(fds[i], &copy)) {
			return fds[i];
		}
	}
	return -1;
}

void event_timer_dealloc(int timer) {
	close(timer);
}

#endif
