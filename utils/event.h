
#ifndef EVENT_H
#define EVENT_H

#include <sys/types.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>

fd_set s;
struct itimerspec v;

void event_init(void) {
  FD_ZERO(&s);
}

void timer_reset(int);

int timer_add(int sec, int nsec) {
  int timer = timerfd_create(CLOCK_REALTIME, 0);
  v = (struct itimerspec) {
    .it_interval = {0, 0},
    .it_value    = {sec, nsec},
  };
  timer_reset(timer);
  FD_SET(timer, &s);
  return timer;
}

void timer_reset(int timer) {
  timerfd_settime(timer, 0, &v, NULL);
}

int timer_wait(int timer) {
  if (select(FD_SETSIZE, &s, NULL, NULL, NULL) < 0) {
    exit(EXIT_FAILURE);
  }
  if(!FD_ISSET(timer, &s)) {
    return -1;
  }
  return 0;
} 

void timer_dealloc(int timer) {
  close(timer);
}

#endif
