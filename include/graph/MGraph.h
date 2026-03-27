#ifndef INCLUDE_GRAPH_MGRAPH_H
#define INCLUDE_GRAPH_MGRAPH_H

#include "GraphOps.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MGraph {
  BaseGraph bg;
  int n_verts;
  int n_edges;
  void** verts;
  bool directed;
  Weight* adj;
} MGraph;

MGraph* mgraph_init(int capacity);
void mgraph_destroy(MGraph* g);
#ifdef __cplusplus
}
#endif

#endif  // !INCLUDE_GRAPH_MGRAPH_H
