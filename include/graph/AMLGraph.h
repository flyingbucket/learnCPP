#ifndef INCLUDE_GRAPH_ALMGRAPH_H
#define INCLUDE_GRAPH_ALMGRAPH_H
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
#include "GraphOps.h"
struct VNode;
struct ENode;
typedef struct VNode {
  void* data;
  struct ENode* firstedge;
} VNode;
typedef struct ENode {
  VertexId ivex, jvex;
  struct ENode* ilink;
  struct ENode* jlink;
  Weight w;
} ENode;

typedef struct {
  VNode* verts;
  BaseGraph bg;
  int n_verts;
  int n_edges;
  int vert_capacity;
} AMLGraph;

AMLGraph* amlgraph_create(int n_verts);
void amlgraph_destroy(AMLGraph* g);

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // !INCLUDE_GRAPH_ALMGRAPH_H
