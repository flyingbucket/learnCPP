#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "graph/GraphAlgorithms.h"
#include "graph/GraphOps.h"
#include "queue/SqQueue.h"

void BFSTraverse(void* graph_context, const BaseGraph* graph_methods,
                 VisitorClosure visitor) {
  if (!graph_context || !graph_context || !visitor.on_edge) return;
  void* G = graph_context;
  const BaseGraph* bg = graph_methods;
  int n = bg->iops.vertex_count(G);
  if (n <= 0) return;
  bool* visited = (bool*)calloc(n, sizeof(bool));
  for (int i = 0; i < n; i++) {
    if (!visited[i]) BFS(G, bg, i, visited, visitor);
  }
}

void BFS(void* graph_context, const BaseGraph* graph_methods, VertexId start_v,
         bool* visited, VisitorClosure visitor) {
  if (!graph_context || !visited) return;

  void* G = graph_context;
  const BaseGraph* bg = graph_methods;
  int n = bg->iops.vertex_count(G);

  SqQueue* q = NULL;
  InitQueue(&q, sizeof(VertexId), n);

  visited[start_v] = true;
  if (visitor.on_vertex)
    visitor.on_vertex(G, bg, start_v, visitor.user_context);
  EnQueue(q, &start_v);

  while (!isSqQueueEmpty(q)) {
    VertexId v;
    DeQueue(q, &v);

    for (VertexId neighbor = bg->qops.first_neighbor(G, v);
         bg->iops.valid_vertex(G, neighbor);
         neighbor = bg->qops.next_neighbor(G, v, neighbor)) {
      Weight w = bg->wops.get_edge_weight(G, v, neighbor);

      if (!visited[neighbor]) {
        visited[neighbor] = true;

        if (visitor.on_edge) {
          visitor.on_edge(G, bg, v, neighbor, w, visitor.user_context);
        }

        if (visitor.on_vertex) {
          visitor.on_vertex(G, bg, neighbor, visitor.user_context);
        }

        EnQueue(q, &neighbor);
      }
    }
  }
  DestorySqQueue(&q);
}

static void on_edge_min_dist(void* G, const BaseGraph* bg, VertexId tail,
                             VertexId head, Weight w, void* ctx) {
  int* d = (int*)ctx;
  d[head] = d[tail] + 1;
  (void)G;
  (void)bg;
  (void)w;
}
static void on_vertex_min_dist(void* G, const BaseGraph* bg, VertexId v,
                               void* ctx) {
  (void)G;
  (void)bg;
  (void)ctx;
  printf("Visited vertex %d\n", v);
}
void BFSCalculateShortestPaths(void* G, const BaseGraph* bg, VertexId start_v,
                               int* out_dists) {
  int n = bg->iops.vertex_count(G);
  bool* visited = calloc(n, sizeof(bool));

  for (int i = 0; i < n; i++) out_dists[i] = -1;
  out_dists[start_v] = 0;

  VisitorClosure visitor = {.on_edge = on_edge_min_dist,
                            .on_vertex = on_vertex_min_dist,
                            .user_context = out_dists};

  BFS(G, bg, start_v, visited, visitor);

  free(visited);
}

void DFSTraverse(void* graph_context, const BaseGraph* graph_methods,
                 VisitorClosure visitor) {
  if (!graph_context || !graph_methods) return;
  void* G = graph_context;
  const BaseGraph* bg = graph_methods;
  int n = bg->iops.vertex_count(G);
  if (n <= 0) return;
  bool* visited = (bool*)calloc(n, sizeof(bool));
  for (VertexId i = 0; i < n; i++) {
    if (!visited[i]) {
      DFS(G, bg, i, visited, visitor);
    }
  }
}

void DFS(void* graph_context, const BaseGraph* graph_methods, VertexId start_v,
         bool* visited, VisitorClosure visitor) {
  if (!graph_context || !graph_methods) return;
  void* G = graph_context;
  const BaseGraph* bg = graph_methods;
  int n = bg->iops.vertex_count(G);
  if (n <= 0 || !bg->iops.valid_vertex(G, start_v)) return;
  visited[start_v] = true;
  if (visitor.on_vertex) {
    visitor.on_vertex(G, bg, start_v, visitor.user_context);
  }
  for (VertexId neighbor = bg->qops.first_neighbor(G, start_v);
       bg->iops.valid_vertex(G, neighbor);
       neighbor = bg->qops.next_neighbor(G, start_v, neighbor)) {
    if (!visited[neighbor]) {
      visited[neighbor] = true;
      if (visitor.on_edge) {
        Weight w = bg->wops.get_edge_weight(G, start_v, neighbor);
        visitor.on_edge(G, bg, start_v, neighbor, w, visitor.user_context);
      }
      DFS(G, bg, neighbor, visited, visitor);
    }
  }
}
