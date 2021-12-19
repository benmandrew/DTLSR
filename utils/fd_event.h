
#ifndef FD_EVENT_H
#define FD_EVENT_H

#include <sys/types.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

#include "logging.h"

fd_set s;

typedef struct Timer {
	int fd;
	struct itimerspec v;
} Timer;

void event_init(void);

void event_append(int fd);

void event_timer_reset(Timer* t);

void event_timer_disarm(Timer* t);

void event_timer_arm(Timer* t, int sec, int nsec);

Timer event_timer_append(int sec, int nsec);

int event_wait(int* fds, int n_fds);

void event_timer_dealloc(Timer timer);

#endif
