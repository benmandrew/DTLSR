
#ifndef CAPTURE_H
#define CAPTURE_H

#include <pcap.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef unsigned int u_int;
typedef unsigned short u_short;
typedef unsigned char u_char;

struct capture_info {
  char *dev;
  pcap_t *handle;
  struct bpf_program fp;
  char has_fp;
};

struct capture_info capture_start(char* dev);

void capture_flush(struct capture_info cap_info);

#endif
