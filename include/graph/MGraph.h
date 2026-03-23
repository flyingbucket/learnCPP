#ifndef INCLUDE_GRAPH_MGRAPH_H
#define INCLUDE_GRAPH_MGRAPH_H

#include "GraphOps.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MGraph {
  BaseGraph bg;
  int n_verts;
  void** verts;
  Weight* adj;
} MGraph;

MGraph* mgraph_create(int n_verts);
void mgraph_destroy(MGraph* g);
#ifdef __cplusplus
}
#endif

#endif  // !INCLUDE_GRAPH_MGRAPH_H
