
#ifndef TEST_INCLUDES
#define TEST_INCLUDES
#include <check.h>
#include <stdlib.h>
#endif

#include "algorithm/pathfind.h"

/*
 * Generate pentagonal graph
 * (seen S)
 *
 *    S2
 *   /  \
 * S1    S3
 *  |    |
 * S5 -- S4
 */
START_TEST(test_pathfind_pentagon) {
  Node graph[MAX_NODE_NUM];
  graph_init(graph);
  node_init(&graph[0], 2);
  graph[0].neighbour_ids[0] = 5;
  graph[0].neighbour_ids[1] = 2;
  graph[0].neighbour_ips[0] = inet_addr("10.0.4.1");
  graph[0].neighbour_ips[1] = inet_addr("10.0.0.2");
  graph[0].source_ips[0] = inet_addr("10.0.4.2");
  graph[0].source_ips[1] = inet_addr("10.0.0.1");
  graph[0].link_statuses[0] = LINK_UP;
  graph[0].link_statuses[1] = LINK_UP;
  node_init(&graph[1], 2);
  graph[1].neighbour_ids[0] = 1;
  graph[1].neighbour_ids[1] = 3;
  graph[1].neighbour_ips[0] = inet_addr("10.0.0.1");
  graph[1].neighbour_ips[1] = inet_addr("10.0.1.2");
  graph[1].source_ips[0] = inet_addr("10.0.0.2");
  graph[1].source_ips[1] = inet_addr("10.0.1.1");
  graph[1].link_statuses[0] = LINK_UP;
  graph[1].link_statuses[1] = LINK_UP;
  node_init(&graph[2], 2);
  graph[2].neighbour_ids[0] = 2;
  graph[2].neighbour_ids[1] = 4;
  graph[2].neighbour_ips[0] = inet_addr("10.0.1.1");
  graph[2].neighbour_ips[1] = inet_addr("10.0.2.2");
  graph[2].source_ips[0] = inet_addr("10.0.1.2");
  graph[2].source_ips[1] = inet_addr("10.0.2.1");
  graph[2].link_statuses[0] = LINK_UP;
  graph[2].link_statuses[1] = LINK_UP;
  node_init(&graph[3], 2);
  graph[3].neighbour_ids[0] = 3;
  graph[3].neighbour_ids[1] = 5;
  graph[3].neighbour_ips[0] = inet_addr("10.0.2.1");
  graph[3].neighbour_ips[1] = inet_addr("10.0.3.2");
  graph[3].source_ips[0] = inet_addr("10.0.2.2");
  graph[3].source_ips[1] = inet_addr("10.0.3.1");
  graph[3].link_statuses[0] = LINK_UP;
  graph[3].link_statuses[1] = LINK_UP;
  node_init(&graph[4], 2);
  graph[4].neighbour_ids[0] = 4;
  graph[4].neighbour_ids[1] = 1;
  graph[4].neighbour_ips[0] = inet_addr("10.0.3.1");
  graph[4].neighbour_ips[1] = inet_addr("10.0.4.2");
  graph[4].source_ips[0] = inet_addr("10.0.3.2");
  graph[4].source_ips[1] = inet_addr("10.0.4.1");
  graph[4].link_statuses[0] = LINK_UP;
  graph[4].link_statuses[1] = LINK_UP;
  struct hop_dest next_hops_n1[MAX_NODE_NUM];
  memset(next_hops_n1, 0, sizeof next_hops_n1);
  struct hop_dest next_hops_n3[MAX_NODE_NUM];
  memset(next_hops_n3, 0, sizeof next_hops_n3);
  pathfind(graph, 1, next_hops_n1);
  pathfind(graph, 3, next_hops_n3);

  ck_assert_int_eq(next_hops_n1[0].next_hop, -1);
  ck_assert_uint_eq(next_hops_n1[0].dest_ip, inet_addr("0.0.0.0"));
  ck_assert_int_eq(next_hops_n1[1].next_hop, 2);
  ck_assert_uint_eq(next_hops_n1[1].dest_ip, inet_addr("10.0.0.2"));
  ck_assert_int_eq(next_hops_n1[2].next_hop, 2);
  ck_assert_uint_eq(next_hops_n1[2].dest_ip, inet_addr("10.0.1.2"));
  ck_assert_int_eq(next_hops_n1[3].next_hop, 5);
  ck_assert_uint_eq(next_hops_n1[3].dest_ip, inet_addr("10.0.3.1"));
  ck_assert_int_eq(next_hops_n1[4].next_hop, 5);
  ck_assert_uint_eq(next_hops_n1[4].dest_ip, inet_addr("10.0.4.1"));
  for (int i = 5; i < MAX_NODE_NUM; i++) {
    ck_assert_int_eq(next_hops_n1[i].next_hop, -1);
    ck_assert_uint_eq(next_hops_n1[i].dest_ip, inet_addr("0.0.0.0"));
  }

  ck_assert_int_eq(next_hops_n3[0].next_hop, 2);
  ck_assert_uint_eq(next_hops_n3[0].dest_ip, inet_addr("10.0.0.1"));
  ck_assert_int_eq(next_hops_n3[1].next_hop, 2);
  ck_assert_uint_eq(next_hops_n3[1].dest_ip, inet_addr("10.0.1.1"));
  ck_assert_int_eq(next_hops_n3[2].next_hop, -1);
  ck_assert_uint_eq(next_hops_n3[2].dest_ip, inet_addr("0.0.0.0"));
  ck_assert_int_eq(next_hops_n3[3].next_hop, 4);
  ck_assert_uint_eq(next_hops_n3[3].dest_ip, inet_addr("10.0.2.2"));
  ck_assert_int_eq(next_hops_n3[4].next_hop, 4);
  ck_assert_uint_eq(next_hops_n3[4].dest_ip, inet_addr("10.0.3.2"));
  for (int i = 5; i < MAX_NODE_NUM; i++) {
    ck_assert_int_eq(next_hops_n3[i].next_hop, -1);
    ck_assert_uint_eq(next_hops_n3[i].dest_ip, inet_addr("0.0.0.0"));
  }
}
END_TEST

/*
 * Generate graph with partition
 * (seen S)
 *
 * S1 -- S2 -- S3
 *
 *    S4 -- S5
 */
START_TEST(test_pathfind_partition) {
  Node graph[MAX_NODE_NUM];
  graph_init(graph);
  node_init(&graph[0], 1);
  graph[0].neighbour_ids[0] = 2;
  graph[0].neighbour_ips[0] = inet_addr("10.0.0.2");
  graph[0].source_ips[0] = inet_addr("10.0.0.1");
  graph[0].link_statuses[0] = LINK_UP;
  node_init(&graph[1], 2);
  graph[1].neighbour_ids[0] = 1;
  graph[1].neighbour_ids[1] = 3;
  graph[1].neighbour_ips[0] = inet_addr("10.0.0.1");
  graph[1].neighbour_ips[1] = inet_addr("10.0.1.2");
  graph[1].source_ips[0] = inet_addr("10.0.0.2");
  graph[1].source_ips[1] = inet_addr("10.0.1.1");
  graph[1].link_statuses[0] = LINK_UP;
  graph[1].link_statuses[1] = LINK_UP;
  node_init(&graph[2], 1);
  graph[2].neighbour_ids[0] = 2;
  graph[2].neighbour_ips[0] = inet_addr("10.0.1.1");
  graph[2].source_ips[0] = inet_addr("10.0.1.2");
  graph[2].link_statuses[0] = LINK_UP;
  node_init(&graph[3], 1);
  graph[3].neighbour_ids[0] = 5;
  graph[3].neighbour_ips[0] = inet_addr("10.0.2.2");
  graph[3].source_ips[0] = inet_addr("10.0.2.1");
  graph[3].link_statuses[0] = LINK_UP;
  node_init(&graph[4], 1);
  graph[4].neighbour_ids[0] = 4;
  graph[4].neighbour_ips[0] = inet_addr("10.0.2.1");
  graph[4].source_ips[0] = inet_addr("10.0.2.2");
  graph[4].link_statuses[0] = LINK_UP;
  struct hop_dest next_hops_top[MAX_NODE_NUM];
  memset(next_hops_top, 0, sizeof next_hops_top);
  struct hop_dest next_hops_bottom[MAX_NODE_NUM];
  memset(next_hops_bottom, 0, sizeof next_hops_bottom);
  pathfind(graph, 3, next_hops_top);
  pathfind(graph, 5, next_hops_bottom);

  ck_assert_int_eq(next_hops_top[0].next_hop, 2);
  ck_assert_uint_eq(next_hops_top[0].dest_ip, inet_addr("10.0.0.1"));
  ck_assert_int_eq(next_hops_top[1].next_hop, 2);
  ck_assert_uint_eq(next_hops_top[1].dest_ip, inet_addr("10.0.1.1"));
  for (int i = 2; i < MAX_NODE_NUM; i++) {
    ck_assert_int_eq(next_hops_top[i].next_hop, -1);
    ck_assert_uint_eq(next_hops_top[i].dest_ip, inet_addr("0.0.0.0"));
  }
  ck_assert_int_eq(next_hops_bottom[0].next_hop, -1);
  ck_assert_uint_eq(next_hops_bottom[0].dest_ip, inet_addr("0.0.0.0"));
  ck_assert_int_eq(next_hops_bottom[1].next_hop, -1);
  ck_assert_uint_eq(next_hops_bottom[1].dest_ip, inet_addr("0.0.0.0"));
  ck_assert_int_eq(next_hops_bottom[2].next_hop, -1);
  ck_assert_uint_eq(next_hops_bottom[2].dest_ip, inet_addr("0.0.0.0"));
  ck_assert_int_eq(next_hops_bottom[3].next_hop, 4);
  ck_assert_uint_eq(next_hops_bottom[3].dest_ip, inet_addr("10.0.2.1"));
  for (int i = 4; i < MAX_NODE_NUM; i++) {
    ck_assert_int_eq(next_hops_bottom[i].next_hop, -1);
    ck_assert_uint_eq(next_hops_bottom[i].dest_ip, inet_addr("0.0.0.0"));
  }
}
END_TEST

/*
 * Generate graph with opaque nodes
 * (seen S, opaque O)
 *
 * S1 -- S3 -- O4
 *  |          |
 * O2          S5
 */
START_TEST(test_pathfind_opaque) {
  Node graph[MAX_NODE_NUM];
  graph_init(graph);
  node_init(&graph[0], 2);
  graph[0].neighbour_ids[0] = 2;
  graph[0].neighbour_ids[1] = 3;
  graph[0].neighbour_ips[0] = inet_addr("10.0.0.1");
  graph[0].neighbour_ips[1] = inet_addr("10.0.1.2");
  graph[0].source_ips[0] = inet_addr("10.0.0.2");
  graph[0].source_ips[1] = inet_addr("10.0.1.1");
  graph[0].link_statuses[0] = LINK_UP;
  graph[0].link_statuses[1] = LINK_UP;
  graph[1].state = NODE_OPAQUE;
  node_init(&graph[2], 2);
  graph[2].neighbour_ids[0] = 1;
  graph[2].neighbour_ids[1] = 4;
  graph[2].neighbour_ips[0] = inet_addr("10.0.1.1");
  graph[2].neighbour_ips[1] = inet_addr("10.0.2.2");
  graph[2].source_ips[0] = inet_addr("10.0.1.2");
  graph[2].source_ips[1] = inet_addr("10.0.2.1");
  graph[2].link_statuses[0] = LINK_UP;
  graph[2].link_statuses[1] = LINK_UP;
  graph[3].state = NODE_OPAQUE;
  node_init(&graph[4], 1);
  graph[4].neighbour_ids[0] = 4;
  graph[4].neighbour_ips[0] = inet_addr("10.0.3.1");
  graph[4].source_ips[0] = inet_addr("10.0.3.2");
  graph[4].link_statuses[0] = LINK_UP;
  struct hop_dest next_hops[MAX_NODE_NUM];
  memset(next_hops, 0, sizeof next_hops);
  pathfind(graph, 1, next_hops);

  ck_assert_int_eq(next_hops[0].next_hop, -1);
  ck_assert_uint_eq(next_hops[0].dest_ip, inet_addr("0.0.0.0"));
  ck_assert_int_eq(next_hops[1].next_hop, 2);
  ck_assert_uint_eq(next_hops[1].dest_ip, inet_addr("10.0.0.1"));
  ck_assert_int_eq(next_hops[2].next_hop, 3);
  ck_assert_uint_eq(next_hops[2].dest_ip, inet_addr("10.0.1.2"));
  ck_assert_int_eq(next_hops[3].next_hop, 3);
  ck_assert_uint_eq(next_hops[3].dest_ip, inet_addr("10.0.2.2"));
  for (int i = 4; i < MAX_NODE_NUM; i++) {
    ck_assert_int_eq(next_hops[i].next_hop, -1);
    ck_assert_uint_eq(next_hops[i].dest_ip, inet_addr("0.0.0.0"));
  }
}
END_TEST
