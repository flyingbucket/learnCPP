#ifndef INCLUDE_GRAPH_ALGRAPH_H
#define INCLUDE_GRAPH_ALGRAPH_H
#include <stdbool.h>

#include "GraphOps.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

struct ALVNode;
struct ALENode;

typedef struct ALVNode {
  void* data;
  VertexId id;
  struct ALENode* firstarc;
} ALVNode, *AdjList;

typedef struct ALENode {
  VertexId adjvex;
  struct ALENode* nextarc;
  Weight w;
} ALENode;

typedef struct {
  BaseGraph bg;
  AdjList verts;
  int n_verts, n_edges;
  int vert_capacity;
  bool directed;
} ALGraph;

ALGraph* algraph_init(int capacity);
void algraph_destroy(ALGraph* G);

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // !INCLUDE_GRAPH_ALGRAPH_H
