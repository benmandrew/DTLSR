
#ifndef ARP_H
#define ARP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#include "logging.h"

u_int8_t get_neighbour_ips(long** ips);

#endif
