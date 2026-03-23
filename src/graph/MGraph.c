#include "graph/MGraph.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph/GraphOps.h"

static int m_vertex_count(void* G) {
  MGraph* g = (MGraph*)G;
  return g->n_verts;
}
static bool m_valid_vertex(void* G, VertexId v) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  if (v >= 0 && v < n)
    return true;
  else
    return false;
}
static bool m_direct(void* G) {
  (void)G;
  printf("Error: Not implemented");
  return true;
}
static const GraphInfoOps MGRAPH_IOPS = {
    .vertex_count = m_vertex_count,
    .valid_vertex = m_valid_vertex,
    .directed = m_direct,
};

static bool m_adjacent(void* G, VertexId v1, VertexId v2) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  Weight(*adj)[n] = (Weight(*)[n])g->adj;
  return adj[v1][v2] != 0;
}
static int m_first_neighbor(void* G, VertexId v) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  if (v > n * n) return -1;
  for (int w = 0; w < n; ++w) {
    if (g->adj[v * n + w] != 0) return w;
  }
  return -1;
}
static int m_next_neighbor(void* G, VertexId v, VertexId w) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  for (int x = w + 1; x < n; ++x) {
    if (g->adj[v * n + x] != 0) return x;
  }
  return -1;
}
static const GraphQueryOps MGRAPH_QOPS = {
    .adjacent = m_adjacent,
    .first_neighbor = m_first_neighbor,
    .next_neighbor = m_next_neighbor,
};

static VertexId m_add_vert(void* G) {
  printf("Error: Not implemented");
  (void)G;
  return -1;
}
static bool m_delete_vert(void* G, VertexId v) {
  printf("Error: Not implemented");
  (void)G;
  (void)v;
  return false;
}
static bool m_add_edge(void* G, VertexId v1, VertexId v2, Weight w) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  if (g->bg.iops->valid_vertex(G, v1) && g->bg.iops->valid_vertex(G, v2) &&
      v1 != v2) {
    g->adj[v1 * n + v2] = w;
    return true;
  }
  return false;
}
static bool m_remove_edge(void* G, VertexId v1, VertexId v2) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  if (g->bg.iops->valid_vertex(G, v1) && g->bg.iops->valid_vertex(G, v2) &&
      v1 != v2) {
    g->adj[v1 * n + v2] = 0;
    return true;
  }
  return false;
}
static const GraphMutateOps MGRAPH_MOPS = {
    .add_vert = m_add_vert,
    .delete_vert = m_delete_vert,
    .add_edge = m_add_edge,
    .remove_edge = m_remove_edge,
};

static Weight m_update_edge_weight(void* G, VertexId v1, VertexId v2,
                                   Weight w) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  Weight old = g->bg.wops->get_edge_weight(G, v1, v2);
  g->adj[v1 * n + v2] = w;
  return old;
}
static Weight m_get_edge_weight(void* G, VertexId v1, VertexId v2) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  if (!g->bg.iops->valid_vertex(G, v1) || !g->bg.iops->valid_vertex(G, v1) ||
      v1 == v2)
    return -1;
  Weight w = g->adj[v1 * n + v2];
  return w;
}
static const WeightedGraphOps MGRAPH_WOPS = {
    .get_edge_weight = m_get_edge_weight,
    .update_edge_weight = m_update_edge_weight,
};

MGraph* mgraph_create(int n_verts) {
  MGraph* g = (MGraph*)malloc(sizeof(MGraph));
  if (!g) return NULL;

  g->n_verts = n_verts;
  g->verts = NULL;
  g->adj = (Weight*)calloc((size_t)n_verts * (size_t)n_verts, sizeof(Weight));
  if (!g->adj) {
    free(g);
    return NULL;
  }

  g->bg.iops = &MGRAPH_IOPS;
  g->bg.qops = &MGRAPH_QOPS;
  g->bg.wops = &MGRAPH_WOPS;
  g->bg.mops = &MGRAPH_MOPS;

  return g;
}

void mgraph_destroy(MGraph* g) {
  if (!g) return;
  free(g->adj);
  free(g);
}
