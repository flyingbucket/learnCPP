#include "graph/MGraph.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph/GraphOps.h"

static int m_vertex_count(const void* G) {
  MGraph* g = (MGraph*)G;
  return g->n_verts;
}
static int m_edge_count(const void* G) {
  if (G == NULL) return -1;
  MGraph* g = (MGraph*)G;
  return g->n_edges;
}
static bool m_valid_vertex(const void* G, VertexId v) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  if (v >= 0 && v < n)
    return true;
  else
    return false;
}
static bool m_direct(const void* G) {
  (void)G;
  printf("Error: Not implemented");
  return true;
}
static const GraphInfoOps MGRAPH_IOPS = {
    .vertex_count = m_vertex_count,
    .edge_count = m_edge_count,
    .valid_vertex = m_valid_vertex,
    .directed = m_direct,
};

static bool m_adjacent(const void* G, VertexId v1, VertexId v2) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  Weight(*adj)[n] = (Weight(*)[n])g->adj;
  return adj[v1][v2] != 0;
}
static Edge m_first_neighbor(const void* G, VertexId v) {
  Edge invalid_edge = {.t = -1, .h = -1, .w = INFINITY};
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  if (v > n * n) return invalid_edge;
  for (int h = 0; h < n; ++h) {
    if (g->adj[v * n + h] != 0) {
      Edge res = {
          .t = v,
          .h = h,
          .w = g->adj[v * n + h],
      };
      return res;
    };
  }
  return invalid_edge;
}
static Edge m_next_neighbor(const void* G, VertexId v, VertexId w) {
  Edge invalid_edge = {.t = -1, .h = -1, .w = INFINITY};
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  for (int x = w + 1; x < n; ++x) {
    if (g->adj[v * n + x] != 0) {
      Edge res = {
          .t = v,
          .h = x,
          .w = g->adj[v * n + x],
      };
      return res;
    }
  }
  return invalid_edge;
}

static Edge* m_all_edges(const void* G) {
  if (!G) return NULL;
  MGraph* g = (MGraph*)G;

  Edge* edges = (Edge*)malloc(sizeof(Edge) * g->n_edges);
  if (!edges) return NULL;

  int count = 0;
  for (int i = 0; i < g->n_verts; i++) {
    int start_j = (g->directed) ? 0 : i + 1;

    for (int j = start_j; j < g->n_verts; j++) {
      // 3. 索引计算：i * 宽度 + j
      // 注意：这里假设你的矩阵宽度就是 g->n_verts
      Weight w = g->adj[i * g->n_verts + j];

      // 4. 判断边是否存在 (假设 0 代表无边，且不是自环)
      // 你可以根据你的定义修改判断条件，如 w != INF
      if (w > 0 && w < 1000000) {  // 这里的 1000000 对应你的 INF 定义
        if (count < g->n_edges) {
          edges[count].t = i;  // 起点 (tail)
          edges[count].h = j;  // 终点 (head)
          edges[count].w = w;  // 权重
          count++;
        }
      }
    }
  }

  // 5. 安全检查：如果实际发现的边数少于 n_edges，可以更新或记录
  return edges;
}
static const GraphQueryOps MGRAPH_QOPS = {
    .adjacent = m_adjacent,
    .first_neighbor = m_first_neighbor,
    .next_neighbor = m_next_neighbor,
    .all_edges = m_all_edges,
};

static VertexId m_add_vert(void* G) {
  printf(
      "Warning: MGraph does not support dynamic scaling. This function does "
      "nothing.\nIf you are calling this function in the init process, it's "
      "all fine because all information is stored in the adjcancy matrix and "
      "added to the graph through function `add_edge`.\n");
  (void)G;
  return -1;
}
static bool m_delete_vert(void* G, VertexId v) {
  printf(
      "Warning: MGraph does not support dynamic expansion. This function does "
      "nothing\n");
  (void)G;
  (void)v;
  return false;
}
static bool m_add_edge(void* G, VertexId v1, VertexId v2, Weight w) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  if (g->bg.iops.valid_vertex(G, v1) && g->bg.iops.valid_vertex(G, v2) &&
      v1 != v2) {
    g->adj[v1 * n + v2] = w;
    return true;
  }
  return false;
}
static bool m_remove_edge(void* G, VertexId v1, VertexId v2) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  if (g->bg.iops.valid_vertex(G, v1) && g->bg.iops.valid_vertex(G, v2) &&
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
  Weight old = g->bg.wops.get_edge_weight(G, v1, v2);
  g->adj[v1 * n + v2] = w;
  return old;
}
static Weight m_get_edge_weight(const void* G, VertexId v1, VertexId v2) {
  MGraph* g = (MGraph*)G;
  int n = g->n_verts;
  if (!g->bg.iops.valid_vertex(G, v1) || !g->bg.iops.valid_vertex(G, v1) ||
      v1 == v2)
    return -1;
  Weight w = g->adj[v1 * n + v2];
  return w;
}
static const WeightedGraphOps MGRAPH_WOPS = {
    .get_edge_weight = m_get_edge_weight,
    .update_edge_weight = m_update_edge_weight,
};

MGraph* mgraph_init(int n_verts) {
  MGraph* g = (MGraph*)malloc(sizeof(MGraph));
  if (!g) return NULL;

  g->n_verts = n_verts;
  g->n_edges = 0;
  g->verts = NULL;
  g->adj = (Weight*)calloc((size_t)n_verts * (size_t)n_verts, sizeof(Weight));
  if (!g->adj) {
    free(g);
    return NULL;
  }

  BaseGraph initial_bg = {.iops = MGRAPH_IOPS,
                          .qops = MGRAPH_QOPS,
                          .mops = MGRAPH_MOPS,
                          .wops = MGRAPH_WOPS};

  g->bg = initial_bg;
  return g;
}

void mgraph_destroy(MGraph* g) {
  if (!g) return;
  free(g->adj);
  free(g);
}
