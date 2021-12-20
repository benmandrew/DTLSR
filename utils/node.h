
#ifndef NODE_H
#define NODE_H

#include <stdint.h>
#include <stdlib.h>

#include "fd_event.h"

/* Graph representation of an IP node
 * 'neighbour_ips' must be sorted
 */
typedef struct node {
  int id;
  uint32_t* source_ips;
  uint32_t* neighbour_ips;
  // Whether the link is alive or we have detected a breakage
  char* neighbour_links_alive;
  int n_neighbours;

  long timestamp;
} Node;

typedef struct local_node {
  Node node;
  Timer* timers;
  char** interfaces;
} LocalNode;

Node alloc_node(int n);

LocalNode alloc_local_node(int n);

// /* Take two sorted sets and merge them together to form another sorted set
//  * Assuming no common values between the sets, n1 ∩ n2 = ∅
//  */
// static long* neighbour_union(long* n1, u_int8_t n_n1, long* n2, u_int8_t n_n2) {
//   long* n = (long*) malloc((n_n1 + n_n2) * sizeof(long));
//   u_int8_t i = 0, j = 0;
//   for (u_int8_t k = 0; k < n_n1 + n_n2; k++) {
//     // C has short-circuit conditional evaluation so this is memory-safe
//     if (i < n_n1 && (j >= n_n2 || n1[i] <= n2[j])) {
//       n[k] = n1[i];
//       i++;
//     } else {
//       n[k] = n2[j];
//       j++;
//     }
//   }
//   free(n1);
//   free(n2);
//   return n;
// }

// /* Merge a sorted set of neighbour IPs into the existing set
//  * This merging is destructive on 'ns'
//  */
// void node_add_neighbours(Node* node, long* ns, u_int8_t n_ns) {
//   node->neighbour_ips = neighbour_union(node->neighbour_ips, node->n_neighbours, ns, n_ns);;
//   node->n_neighbours += n_ns;
// }

#endif
