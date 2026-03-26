#ifndef INCLUDE_GRAPH_GRAPHALGORITHM_H
#define INCLUDE_GRAPH_GRAPHALGORITHM_H
#include <stdbool.h>
#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
#include "GraphOps.h"

// traverse

typedef struct {
  void (*on_edge)(void* G, const BaseGraph* bg, VertexId tail, VertexId head,
                  Weight w, void* ctx);
  void (*on_vertex)(void* G, const BaseGraph* bg, VertexId v, void* ctx);
  void* user_context;
} VisitorClosure;

void BFSTraverse(void* graph_context, const BaseGraph* graph_methods,
                 VisitorClosure visitor);

void BFS(void* graph_context, const BaseGraph* graph_methods, VertexId start_v,
         bool* visited, VisitorClosure visitor);

void BFSCalculateShortestPaths(void* G, const BaseGraph* bg, VertexId start_v,
                               int* out_dists);

void DFSTraverse(void* graph_context, const BaseGraph* graph_methods,
                 VisitorClosure visitor);
void DFS(void* graph_context, const BaseGraph* graph_methods, VertexId start_v,
         bool* visited, VisitorClosure visitor);

// mst

typedef struct {
  int component_count;
  Edge* edges;
  int* compunent_offset;
} MSTResult;

MSTResult* Prim(void* graph_ori, const BaseGraph* graph_ori_methods);
MSTResult* Krustarl(void* graph_ori, const BaseGraph* graph_ori_methods);
void Dijkstra(void);

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // !INCLUDE_GRAPH_GRAPHALGORITHM_H
