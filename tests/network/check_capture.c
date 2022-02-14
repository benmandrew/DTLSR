
#include "test_inc.h"
#include "network/capture_pi.h"

#include "algorithm/node.h"
#include "algorithm/pathfind.h"

START_TEST(test_capture_generate_exclude_incoming_1) {
  LocalNode n = node_local_alloc(1, 1, 3);
  n.node.source_ips[0] = inet_addr("123.3.4.1");
  char *fexp = generate_exclude_incoming(&n);

  ck_assert_str_eq(fexp,
    "( not ( dst net 123.3.4.1 ) )");

  node_local_dealloc(&n);
  free(fexp);
}
END_TEST

START_TEST(test_capture_generate_exclude_incoming_6) {
  LocalNode n = node_local_alloc(1, 6, 3);
  n.node.source_ips[0] = inet_addr("192.168.4.1");
  n.node.source_ips[1] = inet_addr("192.168.1.5");
  n.node.source_ips[2] = inet_addr("192.168.1.4");
  n.node.source_ips[3] = inet_addr("192.168.1.2");
  n.node.source_ips[4] = inet_addr("192.168.2.1");
  n.node.source_ips[5] = inet_addr("192.168.1.8");
  char *fexp = generate_exclude_incoming(&n);

  ck_assert_str_eq(fexp,
    "( not ( dst net 192.168.4.1 or 192.168.1.5 or 192.168.1.4 or 192.168.1.2 or 192.168.2.1 or 192.168.1.8 ) )");

  node_local_dealloc(&n);
  free(fexp);
}
END_TEST

START_TEST(test_capture_generate_addresses_on_interface) {
  LocalNode n = node_local_alloc(1, 2, 3);
  n.interfaces[0] = "eth0";
  n.interfaces[1] = "eth1";
  n.interfaces[2] = "eth2";
  n.node.neighbour_ids[0] = 2;
  n.node.neighbour_ids[1] = 3;
  n.node.neighbour_ids[2] = 4;
  struct hop_dest next_hops[MAX_NODE_NUM];
  memset(next_hops, 0, sizeof(next_hops));
  next_hops[0].next_hop = 3;
  next_hops[0].dest_ip = (uint32_t)inet_addr("10.0.0.1");
  next_hops[1].next_hop = 2;
  next_hops[1].dest_ip = (uint32_t)inet_addr("10.0.5.9");
  next_hops[2].next_hop = 3;
  next_hops[2].dest_ip = (uint32_t)inet_addr("10.5.1.5");
  next_hops[3].next_hop = 4;
  next_hops[3].dest_ip = (uint32_t)inet_addr("10.4.1.5");
  next_hops[4].next_hop = 4;
  next_hops[4].dest_ip = (uint32_t)inet_addr("10.4.1.2");
  next_hops[5].next_hop = 4;
  next_hops[5].dest_ip = (uint32_t)inet_addr("10.4.1.1");
  char *eth0_up = generate_addresses_on_interface(&n, "eth0", next_hops);
  char *eth1_up = generate_addresses_on_interface(&n, "eth1", next_hops);
  char *eth2_up = generate_addresses_on_interface(&n, "eth2", next_hops);

  ck_assert_str_eq(eth0_up, "( dst net 10.0.5.0/24 )");
  ck_assert_str_eq(eth1_up, "( dst net 10.0.0.0/24 or 10.5.1.0/24 )");
  ck_assert_str_eq(eth2_up, "( dst net 10.4.1.0/24 or 10.4.1.0/24 or 10.4.1.0/24 )");

  node_local_dealloc(&n);
  free(eth0_up);
  free(eth1_up);
  free(eth2_up);
}
END_TEST

// START_TEST(test_capture_generate_incoming_filter_exp) {
//   LocalNode n = node_local_alloc(1, 3, 3);
//   n.node.source_ips[0] = inet_addr("192.168.4.1");
//   n.node.source_ips[1] = inet_addr("192.168.2.1");
//   n.node.source_ips[2] = inet_addr("192.168.1.8");
//   n.interfaces[0] = "eth0";
//   n.interfaces[1] = "eth1";
//   n.interfaces[2] = "eth2";
//   n.node.neighbour_ids[0] = 2;
//   n.node.neighbour_ids[1] = 3;
//   n.node.neighbour_ids[2] = 4;
//   struct hop_dest next_hops[MAX_NODE_NUM];
//   memset(next_hops, 0, sizeof(next_hops));
//   next_hops[0].next_hop = 2;
//   next_hops[0].dest_ip = (uint32_t)inet_addr("10.0.0.1");
//   next_hops[1].next_hop = 2;
//   next_hops[1].dest_ip = (uint32_t)inet_addr("10.5.1.5");
//   char *fexp = generate_incoming_filter_exp(&n, "eth0", next_hops);

//   ck_assert_str_eq(fexp,
//     "( not ( dst net 192.168.4.1 or 192.168.2.1 or 192.168.1.8 ) ) and ( dst net 10.0.0.0/24 or 10.5.1.0/24 )");

//   node_local_dealloc(&n);
//   free(fexp);
// }
// END_TEST

START_TEST(test_capture_generate_replay_command) {
  LocalNode n = node_local_alloc(1, 2, 3);
  n.interfaces[0] = "eth0";
  n.interfaces[1] = "eth1";
  n.node.neighbour_ids[0] = 2;
  n.node.neighbour_ids[1] = 3;
  struct hop_dest next_hops[MAX_NODE_NUM];
  memset(next_hops, 0, sizeof(next_hops));
  next_hops[0].next_hop = 3;
  next_hops[0].dest_ip = (uint32_t)inet_addr("10.0.0.1");
  next_hops[1].next_hop = 2;
  next_hops[1].dest_ip = (uint32_t)inet_addr("10.0.5.9");
  next_hops[2].next_hop = 3;
  next_hops[2].dest_ip = (uint32_t)inet_addr("10.5.1.5");
  char *eth0_up = generate_replay_command(&n, "eth0", next_hops);
  char *eth1_up = generate_replay_command(&n, "eth1", next_hops);

  ck_assert_str_eq(eth0_up, "tcpdump -U -r 'dump_0.pcap' -w- '( dst net 10.0.5.0/24 )' | tcpreplay --topspeed -ieth0 -");
  ck_assert_str_eq(eth1_up, "tcpdump -U -r 'dump_0.pcap' -w- '( dst net 10.0.0.0/24 or 10.5.1.0/24 )' | tcpreplay --topspeed -ieth1 -");

  node_local_dealloc(&n);
  free(eth0_up);
  free(eth1_up);
}
END_TEST

// START_TEST(test_capture_generate_remove_command) {
//   LocalNode n = node_local_alloc(1, 2, 3);
//   n.interfaces[0] = "eth0";
//   n.interfaces[1] = "eth1";
//   n.node.neighbour_ids[0] = 2;
//   n.node.neighbour_ids[1] = 3;
//   struct hop_dest next_hops[MAX_NODE_NUM];
//   memset(next_hops, 0, sizeof(next_hops));
//   next_hops[0].next_hop = 3;
//   next_hops[0].dest_ip = (uint32_t)inet_addr("10.0.0.1");
//   next_hops[1].next_hop = 2;
//   next_hops[1].dest_ip = (uint32_t)inet_addr("10.0.5.9");
//   next_hops[2].next_hop = 3;
//   next_hops[2].dest_ip = (uint32_t)inet_addr("10.5.1.5");
//   char *eth0_up = generate_filter_command(&n, "eth0", next_hops);
//   char *eth1_up = generate_filter_command(&n, "eth1", next_hops);

//   ck_assert_str_eq(eth0_up, "tcpdump -r 'dump.pcap' -w 'dump_tmp.pcap' '( not ( dst net 10.0.5.0/24 ) )'");
//   ck_assert_str_eq(eth1_up, "tcpdump -r 'dump.pcap' -w 'dump_tmp.pcap' '( not ( dst net 10.0.0.0/24 or 10.5.1.0/24 ) )'");

//   node_local_dealloc(&n);
//   free(eth0_up);
//   free(eth1_up);
// }
// END_TEST
