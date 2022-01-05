
#ifndef PATHFIND_H
#define PATHFIND_H

#include "algorithm/def.h"
#include "algorithm/graph.h"
#include "algorithm/heap.h"

int* pathfind(Node* graph, int src_id);

void pathfind_f(Node* graph, int src_id);

#endif