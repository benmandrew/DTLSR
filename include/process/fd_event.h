
#ifndef FD_EVENT_H
#define FD_EVENT_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/timerfd.h>
#include <sys/types.h>
#include <unistd.h>

#include "process/logging.h"

typedef struct Timer {
  int fd;
  struct itimerspec v;
} Timer;

void event_init(void);

void event_append(int fd);

void event_timer_reset(Timer *t);

void event_timer_disarm(Timer *t);

void event_timer_arm(Timer *t, long sec, long nsec);

Timer event_timer_append(long sec, long nsec);

int event_wait(int *fds, int n_fds);

void event_timer_dealloc(Timer timer);

#endif
