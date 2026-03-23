#ifndef INCLUDE_GRAPH_ALGRAPH_H
#define INCLUDE_GRAPH_ALGRAPH_H
#include <stdbool.h>

#include "GraphOps.h"

#ifdef _cplusplus
extern "C" {
#endif  // _cplusplus

struct VNode;
struct ENode;

typedef struct VNode {
  void* data;
  VertexId id;
  struct ENode* firstarc;
} VNode, *AdjList;

typedef struct ENode {
  VertexId adjvex;
  struct ENode* nextarc;
  Weight w;
} ENode;

typedef struct {
  BaseGraph* bg;
  AdjList verts;
  int n_vert, n_edge;
  int vert_capacity;
} ALGraph;

ALGraph* algraph_create(int n_vert, int n_edge);
void algraph_destroy(ALGraph* G);

#ifdef _cplusplus
}
#endif  // _cplusplus
#endif  // !INCLUDE_GRAPH_ALGRAPH_H
