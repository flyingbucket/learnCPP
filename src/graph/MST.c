#include <math.h>
#include <stdlib.h>

#include "graph/GraphAlgorithms.h"
#include "graph/GraphOps.h"

MSTResult* Prim(void* graph_context, const BaseGraph* graph_methods) {
  if (!graph_context || !graph_methods) return NULL;
  void* G = graph_context;
  const BaseGraph* bg = graph_methods;
  int nv = bg->iops.vertex_count(G);
  int ne = bg->iops.edge_count(G);
  MSTResult* result = (MSTResult*)malloc(sizeof(MSTResult));
  result->edges = (Edge*)malloc(sizeof(Edge) * (nv - 1));  // MST 最多 nv-1 条边
  result->component_count = 0;
  result->compunent_offset = NULL;

  bool* in_tree = (bool*)calloc(nv, sizeof(bool));
  Weight* dist_to_tree = (Weight*)malloc(sizeof(Weight) * nv);
  VertexId* parent = (VertexId*)malloc(sizeof(VertexId) * nv);
  for (int i = 0; i < nv; i++) {
    dist_to_tree[i] = INFINITY;
    parent[i] = -1;
  }
}
