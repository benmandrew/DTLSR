
#ifndef GRAPH_H
#define GRAPH_H

#include <stdlib.h>

#include "node.h"

typedef struct Graph {
  Node* nodes;
  size_t n_nodes;
} Graph;

#endif
