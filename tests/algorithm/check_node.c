
#include "test_inc.h"
#include "algorithm/node_pi.h"

START_TEST(test_node_init) {
  Node n;
  node_init(&n, 4);

  ck_assert_int_eq(n.n_neighbours, 4);
}
END_TEST

START_TEST(test_node_local_alloc) {
  LocalNode n = node_local_alloc(1, 6, 5);

  ck_assert_int_eq(n.node.n_neighbours, 6);

  node_local_dealloc(&n);
}
END_TEST

START_TEST(test_node_update_time) {
  Node n;
  node_init(&n, 1);
  struct timeval tv, res;
  gettimeofday(&tv, NULL);
  unsigned long long now = (unsigned long long)(tv.tv_sec) * 1000 +
                           (unsigned long long)(tv.tv_usec) / 1000;
  node_update_time(&n);
  unsigned long long diff = n.timestamp - now;

  // Less than 3 millisecond difference
  // Chosen completely arbitrarily
  ck_assert_int_le(diff, 3);
}
END_TEST
