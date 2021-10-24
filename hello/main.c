
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <sys/uio.h>

void handle_datagram(char* buffer, ssize_t len) {
  
}


int main(int argc, char** argv) {

  const char* hostname = 0; /* wildcard */
  const char* portname = "daytime";
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = 0;
  // hints.ai_flags = AI_PASSIVE|AI_ADDRCONFIG;
  hints.ai_flags = AI_ADDRCONFIG;
  struct addrinfo* res = 0;
  int err = getaddrinfo(hostname, portname, &hints, &res);
  if (err != 0) {
    fprintf(stderr, "failed to resolve local socket address (err=%d)", err);
    exit(-1);
  }

  int fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  if (fd == -1) {
    fprintf(stderr, "%s", strerror(errno));
    exit(-1);
  }

  if (bind(fd, res->ai_addr, res->ai_addrlen) == -1) {
    fprintf(stderr, "%s", strerror(errno));
    exit(-1);
  }

  freeaddrinfo(res);

  char buffer[549];
  struct sockaddr_storage src_addr;
  socklen_t src_addr_len = sizeof(src_addr);
  ssize_t count = recvfrom(fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&src_addr, &src_addr_len);
  if (count == -1) {
    fprintf(stderr, "%s", strerror(errno));
    exit(-1);
  } else if (count == sizeof(buffer)) {
    fprintf(stderr, "datagram too large for buffer: truncated");
  } else {
    handle_datagram(buffer, count);
  }


  return 0;
}














