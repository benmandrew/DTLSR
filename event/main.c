
#include <sys/types.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>


int main(int argc, char** argv) {

  int timer = timerfd_create(CLOCK_REALTIME, 0);

  fd_set s;
  FD_ZERO(&s);

  struct itimerspec v = {
    .it_value    = {1, 0},
    .it_interval = {0, 0},
  };

  timerfd_settime(timer, 0, &v, NULL);
  FD_SET(timer, &s);

  while (1) {
    if (select(FD_SETSIZE, &s, NULL, NULL, NULL) < 0) {
      exit(EXIT_FAILURE);
    }

    if(!FD_ISSET(timer, &s)) {
      continue;
    }

    timerfd_settime(timer, 0, &v, NULL);

    printf("FIRE\n");
  }


  close(timer);

  return 0;
}



