
#include "test_inc.h"
#include "algorithm/pathfind_pi.h"

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

/*
 * Generate linear graph with opaque endpoint
 * (seen S, opaque O)
 * 
 *    30    58    10    = 98
 * S1 -- S2 -- S3 -- O4
 *             |
 *             |  INT16_MAX
 *             O5
 */
START_TEST(test_pathfind_metric) {
  Node graph[MAX_NODE_NUM];
  graph_init(graph);
  node_init(&graph[0], 1);
  graph[0].neighbour_ids[0] = 2;
  graph[0].neighbour_ips[0] = inet_addr("10.0.0.2");
  graph[0].source_ips[0] = inet_addr("10.0.0.1");
  graph[0].link_statuses[0] = LINK_UP;
  graph[0].link_metrics[0] = 30;
  node_init(&graph[1], 2);
  graph[1].neighbour_ids[0] = 1;
  graph[1].neighbour_ids[1] = 3;
  graph[1].neighbour_ips[0] = inet_addr("10.0.0.1");
  graph[1].neighbour_ips[1] = inet_addr("10.0.1.2");
  graph[1].source_ips[0] = inet_addr("10.0.0.2");
  graph[1].source_ips[1] = inet_addr("10.0.1.1");
  graph[1].link_statuses[0] = LINK_UP;
  graph[1].link_statuses[1] = LINK_UP;
  graph[1].link_metrics[0] = 5;
  graph[1].link_metrics[1] = 58;
  node_init(&graph[2], 3);
  graph[2].neighbour_ids[0] = 2;
  graph[2].neighbour_ids[1] = 4;
  graph[2].neighbour_ids[2] = 5;
  graph[2].neighbour_ips[0] = inet_addr("10.0.1.1");
  graph[2].neighbour_ips[1] = inet_addr("10.0.2.2");
  graph[2].neighbour_ips[2] = inet_addr("10.0.3.2");
  graph[2].source_ips[0] = inet_addr("10.0.1.2");
  graph[2].source_ips[1] = inet_addr("10.0.2.1");
  graph[2].source_ips[2] = inet_addr("10.0.3.1");
  graph[2].link_statuses[0] = LINK_UP;
  graph[2].link_statuses[1] = LINK_UP;
  graph[2].link_statuses[2] = LINK_UP;
  graph[2].link_metrics[0] = 800;
  graph[2].link_metrics[1] = 10;
  graph[2].link_metrics[2] = INT16_MAX;
  graph[3].state = NODE_OPAQUE;
  graph[4].state = NODE_OPAQUE;
  struct hop_dest next_hops[MAX_NODE_NUM];
  memset(next_hops, 0, sizeof next_hops);
  pathfind(graph, 1, next_hops);

  ck_assert_int_eq(next_hops[1].metric, 30);
  ck_assert_int_eq(next_hops[2].metric, 88);
  ck_assert_int_eq(next_hops[3].metric, 98);
  ck_assert_int_eq(next_hops[4].metric, INT16_MAX);
}
END_TEST

/*
 * Generate long line graph
 * (seen S, opaque O)
 *
 * O2 -- S3 -- S4 -- S5 -- S6 -- S7 -- S8 -- S9 -- O1
 */
START_TEST(test_pathfind_line) {
  Node graph[MAX_NODE_NUM];
  graph_init(graph);
  node_init(&graph[0], 1);
  graph[0].neighbour_ids[0] = 9;
  graph[0].neighbour_ips[0] = inet_addr("10.0.7.9");
  graph[0].source_ips[0] = inet_addr("10.0.7.1");
  graph[0].link_statuses[0] = LINK_UP;
  graph[0].state = NODE_OPAQUE;
  node_init(&graph[1], 1);
  graph[1].neighbour_ids[0] = 3;
  graph[1].neighbour_ips[0] = inet_addr("10.0.6.3");
  graph[1].source_ips[0] = inet_addr("10.0.6.2");
  graph[1].link_statuses[0] = LINK_UP;
  graph[1].state = NODE_OPAQUE;
  node_init(&graph[2], 2);
  graph[2].neighbour_ids[0] = 2;
  graph[2].neighbour_ids[1] = 4;
  graph[2].neighbour_ips[0] = inet_addr("10.0.6.2");
  graph[2].neighbour_ips[1] = inet_addr("10.0.0.4");
  graph[2].source_ips[0] = inet_addr("10.0.6.3");
  graph[2].source_ips[1] = inet_addr("10.0.0.3");
  graph[2].link_statuses[0] = LINK_UP;
  graph[2].link_statuses[1] = LINK_UP;
  node_init(&graph[3], 2);
  graph[3].neighbour_ids[0] = 3;
  graph[3].neighbour_ids[1] = 5;
  graph[3].neighbour_ips[0] = inet_addr("10.0.0.3");
  graph[3].neighbour_ips[1] = inet_addr("10.0.3.5");
  graph[3].source_ips[0] = inet_addr("10.0.0.4");
  graph[3].source_ips[1] = inet_addr("10.0.3.4");
  graph[3].link_statuses[0] = LINK_UP;
  graph[3].link_statuses[1] = LINK_UP;
  node_init(&graph[4], 2);
  graph[4].neighbour_ids[0] = 4;
  graph[4].neighbour_ids[1] = 6;
  graph[4].neighbour_ips[0] = inet_addr("10.0.3.4");
  graph[4].neighbour_ips[1] = inet_addr("10.0.1.6");
  graph[4].source_ips[0] = inet_addr("10.0.3.5");
  graph[4].source_ips[1] = inet_addr("10.0.1.5");
  graph[4].link_statuses[0] = LINK_UP;
  graph[4].link_statuses[1] = LINK_UP;
  node_init(&graph[5], 2);
  graph[5].neighbour_ids[0] = 5;
  graph[5].neighbour_ids[1] = 7;
  graph[5].neighbour_ips[0] = inet_addr("10.0.1.5");
  graph[5].neighbour_ips[1] = inet_addr("10.0.4.7");
  graph[5].source_ips[0] = inet_addr("10.0.1.6");
  graph[5].source_ips[1] = inet_addr("10.0.4.6");
  graph[5].link_statuses[0] = LINK_UP;
  graph[5].link_statuses[1] = LINK_UP;
  node_init(&graph[6], 2);
  graph[6].neighbour_ids[0] = 6;
  graph[6].neighbour_ids[1] = 8;
  graph[6].neighbour_ips[0] = inet_addr("10.0.4.6");
  graph[6].neighbour_ips[1] = inet_addr("10.0.2.8");
  graph[6].source_ips[0] = inet_addr("10.0.4.7");
  graph[6].source_ips[1] = inet_addr("10.0.2.7");
  graph[6].link_statuses[0] = LINK_UP;
  graph[6].link_statuses[1] = LINK_UP;
  node_init(&graph[7], 2);
  graph[7].neighbour_ids[0] = 7;
  graph[7].neighbour_ids[1] = 9;
  graph[7].neighbour_ips[0] = inet_addr("10.0.2.7");
  graph[7].neighbour_ips[1] = inet_addr("10.0.5.9");
  graph[7].source_ips[0] = inet_addr("10.0.2.8");
  graph[7].source_ips[1] = inet_addr("10.0.5.8");
  graph[7].link_statuses[0] = LINK_UP;
  graph[7].link_statuses[1] = LINK_UP;
  node_init(&graph[8], 2);
  graph[8].neighbour_ids[0] = 8;
  graph[8].neighbour_ids[1] = 1;
  graph[8].neighbour_ips[0] = inet_addr("10.0.5.8");
  graph[8].neighbour_ips[1] = inet_addr("10.0.7.1");
  graph[8].source_ips[0] = inet_addr("10.0.5.9");
  graph[8].source_ips[1] = inet_addr("10.0.7.9");
  graph[8].link_statuses[0] = LINK_UP;
  graph[8].link_statuses[1] = LINK_UP;





  struct hop_dest next_hops_3[MAX_NODE_NUM];
  memset(next_hops_3, 0, sizeof next_hops_3);
  struct hop_dest next_hops_7[MAX_NODE_NUM];
  memset(next_hops_7, 0, sizeof next_hops_7);
  pathfind(graph, 3, next_hops_3);
  pathfind(graph, 7, next_hops_7);

  // ck_assert_int_eq(next_hops_top[0].next_hop, 2);
  // ck_assert_uint_eq(next_hops_top[0].dest_ip, inet_addr("10.0.0.1"));
  // ck_assert_int_eq(next_hops_top[1].next_hop, 2);
  // ck_assert_uint_eq(next_hops_top[1].dest_ip, inet_addr("10.0.1.1"));
  struct in_addr a;
  a.s_addr = next_hops_7[0].dest_ip;
  printf("%d, %s\n", next_hops_7[0].next_hop, inet_ntoa(a));
  a.s_addr = next_hops_7[1].dest_ip;
  printf("%d, %s\n", next_hops_7[1].next_hop, inet_ntoa(a));
  a.s_addr = next_hops_7[2].dest_ip;
  printf("%d, %s\n", next_hops_7[2].next_hop, inet_ntoa(a));
  a.s_addr = next_hops_7[3].dest_ip;
  printf("%d, %s\n", next_hops_7[3].next_hop, inet_ntoa(a));
  a.s_addr = next_hops_7[4].dest_ip;
  printf("%d, %s\n", next_hops_7[4].next_hop, inet_ntoa(a));
  a.s_addr = next_hops_7[5].dest_ip;
  printf("%d, %s\n", next_hops_7[5].next_hop, inet_ntoa(a));
  a.s_addr = next_hops_7[6].dest_ip;
  printf("%d, %s\n", next_hops_7[6].next_hop, inet_ntoa(a));
  a.s_addr = next_hops_7[7].dest_ip;
  printf("%d, %s\n", next_hops_7[7].next_hop, inet_ntoa(a));
  a.s_addr = next_hops_7[8].dest_ip;
  printf("%d, %s\n", next_hops_7[8].next_hop, inet_ntoa(a));







}
END_TEST
