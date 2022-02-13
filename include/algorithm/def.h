
#ifndef DEF_H
#define DEF_H

#include <assert.h>

#define HB_PORT 45000
#define HB_SIZE sizeof(int)
#define LSA_PORT 45001

#define MS2NS 1e+6

// Heartbeat period
#define HEARTBEAT_T 250 * MS2NS
// Add a bit of fudge factor
#define HEARTBEAT_TIMEOUT 2.25 * HEARTBEAT_T
// #define HEARTBEAT_TIMEOUT 1
// How often we recompute the link metric
#define METRIC_RECOMPUTATION_T 1.5 * HEARTBEAT_T

#define LOGGING_ACTIVE

// Maximum number of nodes in the network
#define MAX_NODE_NUM 10
// Maximum number of neighbours
#define MAX_NODE_FAN 6

#endif
