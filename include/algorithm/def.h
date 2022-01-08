
#ifndef DEF_H
#define DEF_H

#include <assert.h>

#define HB_PORT 45000
#define HB_SIZE sizeof(int)
#define LSA_PORT 45001

#define HEARTBEAT_T 2
// A bit of fudge factor
#define HEARTBEAT_TIMEOUT HEARTBEAT_T + 3

#define LOGGING_ACTIVE

// Maximum number of nodes in the network
#define MAX_NODE_NUM 16
// Maximum number of neighbours
#define MAX_NODE_FAN 8

#endif
