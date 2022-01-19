
#ifndef DEF_H
#define DEF_H

#include <assert.h>

#define HB_PORT 45000
#define HB_SIZE sizeof(int)
#define LSA_PORT 45001

// Heartbeat period
#define HEARTBEAT_T 2
// Add a bit of fudge factor
#define HEARTBEAT_TIMEOUT 2 * HEARTBEAT_T + 1
// How often we recompute the link metric
#define METRIC_RECOMPUTATION_T HEARTBEAT_T + 1

#define LOGGING_ACTIVE

// Maximum number of nodes in the network
#define MAX_NODE_NUM 10
// Maximum number of neighbours
#define MAX_NODE_FAN 6

#endif
