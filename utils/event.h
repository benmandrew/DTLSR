
#ifndef EVENT_H
#define EVENT_H

#include <sys/types.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "logging.h"

fd_set s;
struct itimerspec v;

void event_init(void) {
	FD_ZERO(&s);
}

void event_append(int fd) {
	FD_SET(fd, &s);
}

void event_timer_reset(int);

int event_timer_append(int sec, int nsec) {
	int timer = timerfd_create(CLOCK_REALTIME, 0);
	v = (struct itimerspec) {
		.it_interval = {0, 0},
		.it_value    = {sec, nsec},
	};
	event_timer_reset(timer);
	event_append(timer);
	return timer;
}

void event_timer_reset(int timer) {
	timerfd_settime(timer, 0, &v, NULL);
}

int event_wait(int* fds, int n_fds) {
	if (select(FD_SETSIZE, &s, NULL, NULL, NULL) < 0) {
		exit(EXIT_FAILURE);
	}
	for (int i = 0; i < n_fds; i++) {
		if(FD_ISSET(fds[i], &s)) {
			return fds[i];
		}
	}
	return -1;
}

void event_timer_dealloc(int timer) {
	close(timer);
}

#endif
