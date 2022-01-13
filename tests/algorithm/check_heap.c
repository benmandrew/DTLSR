
#include "test_inc.h"
#include "algorithm/heap_pi.h"

START_TEST(test_heap_alloc) {
  MinHeap h = minheap_alloc(64);

  ck_assert_int_eq(h.max_size, 64);
  ck_assert_int_eq(h.size, 0);

  minheap_dealloc(&h);
}
END_TEST

START_TEST(test_heap_build_heap) {
  DijkstraNode *nodes = (DijkstraNode *)malloc(4 * sizeof(DijkstraNode));
  MinHeap h = minheap_alloc(4);
  h.size = 4;
  for (int i = 0; i < 4; i++) {
    nodes[i].id = i + 1;
    nodes[i].tent_dist = 4 - i;
    h.node_ptrs[i] = &nodes[i];
  }
  minheap_build_heap(&h);

  DijkstraNode *v = minheap_pop(&h);
  ck_assert_int_eq(v->id, 4);
  ck_assert_int_eq(v->tent_dist, 1);
  v = minheap_pop(&h);
  ck_assert_int_eq(v->id, 3);
  ck_assert_int_eq(v->tent_dist, 2);
  v = minheap_pop(&h);
  ck_assert_int_eq(v->id, 2);
  ck_assert_int_eq(v->tent_dist, 3);
  v = minheap_pop(&h);
  ck_assert_int_eq(v->id, 1);
  ck_assert_int_eq(v->tent_dist, 4);

  minheap_dealloc(&h);
  free(nodes);
}
END_TEST

START_TEST(test_heap_pop) {
  MinHeap h = minheap_alloc(16);
  DijkstraNode n;
  n.id = 1;
  n.tent_dist = 5;
  minheap_insert(&h, &n);
  ck_assert_int_eq(h.size, 1);
  DijkstraNode *v = minheap_pop(&h);

  ck_assert_int_eq(v->id, 1);
  ck_assert_int_eq(v->tent_dist, 5);
  ck_assert_int_eq(h.size, 0);

  minheap_dealloc(&h);
}
END_TEST

START_TEST(test_heap_insert) {
  MinHeap h = minheap_alloc(4);
  DijkstraNode nodes[3];
  nodes[0].id = 1;
  nodes[0].tent_dist = 10;
  nodes[1].id = 2;
  nodes[1].tent_dist = 0;
  nodes[2].id = 3;
  nodes[2].tent_dist = 5;

  minheap_insert(&h, &nodes[0]);
  ck_assert_int_eq(h.size, 1);
  minheap_insert(&h, &nodes[1]);
  ck_assert_int_eq(h.size, 2);
  minheap_insert(&h, &nodes[2]);
  ck_assert_int_eq(h.size, 3);
  DijkstraNode *v = minheap_pop(&h);
  ck_assert_int_eq(v->id, 2);
  ck_assert_int_eq(v->tent_dist, 0);
  v = minheap_pop(&h);
  ck_assert_int_eq(v->id, 3);
  ck_assert_int_eq(v->tent_dist, 5);
  v = minheap_pop(&h);
  ck_assert_int_eq(v->id, 1);
  ck_assert_int_eq(v->tent_dist, 10);

  minheap_dealloc(&h);
}
END_TEST

START_TEST(test_heap_decrease_dist) {
  MinHeap h = minheap_alloc(16);
  DijkstraNode nodes[4];
  nodes[0].id = 1;
  nodes[0].tent_dist = 5;
  nodes[1].id = 2;
  nodes[1].tent_dist = 10;
  nodes[2].id = 3;
  nodes[2].tent_dist = 20;
  nodes[3].id = 4;
  nodes[3].tent_dist = 15;
  minheap_insert(&h, &nodes[0]);
  minheap_insert(&h, &nodes[1]);
  minheap_insert(&h, &nodes[2]);
  minheap_insert(&h, &nodes[3]);
  minheap_decrease_dist(&h, 2, 0);

  DijkstraNode *v = minheap_pop(&h);
  ck_assert_int_eq(v->id, 2);
  ck_assert_int_eq(v->tent_dist, 0);

  minheap_dealloc(&h);
}
END_TEST
