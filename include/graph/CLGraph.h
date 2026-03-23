#ifndef INCLUDE_GRAPH_CLGRAPH_H
#define INCLUDE_GRAPH_CLGRAPH_H
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
#include "GraphOps.h"

struct VNode;
struct ENode;

typedef struct VNode {
  void* data;
  struct ENode* firstin;
  struct ENode* firstout;
} VNode;

typedef struct ENode {
  Weight w;
  VertexId tailvex;
  VertexId headvex;
  struct ENode* hlink;
  struct ENode* tlink;
} ENode;

typedef struct CLGraph {
  BaseGraph bg;
  VNode* xlist;
  int n_verts;
  int n_edges;
  int vert_capacity;
} CLGraph;

CLGraph* clgraph_create(int n_verts, int n_edges);
void destroy_clgraph(CLGraph* g);
#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // !INCLUDE_GRAPH_CLGRAPH_H
