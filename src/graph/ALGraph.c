#include "graph/ALGraph.h"

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "graph/GraphOps.h"

static int vertex_count(const void* G) {
  if (G == NULL) return -1;
  ALGraph* g = (ALGraph*)G;
  return g->n_verts;
}
static int edge_count(const void* G) {
  if (G == NULL) return -1;
  ALGraph* g = (ALGraph*)G;
  return g->n_edges;
}
static bool valid_vertex(const void* G, VertexId v) {
  if (G == NULL) return false;
  ALGraph* g = (ALGraph*)G;
  if (v >= 0 && v < g->n_verts)
    return true;
  else
    return false;
}
static bool directed(const void* G) {
  assert(G != NULL && "Graph pointer can not be NULL!");
  ALGraph* g = (ALGraph*)G;
  return g->directed;
}

GraphInfoOps const ALGRAPH_IOPS = {
    .vertex_count = vertex_count,
    .edge_count = edge_count,
    .valid_vertex = valid_vertex,
    .directed = directed,
};

static bool adjacent(const void* G, VertexId v1, VertexId v2) {
  if (G == NULL) return false;
  ALGraph* g = (ALGraph*)G;
  if (!g->bg.iops.valid_vertex(G, v1) || !g->bg.iops.valid_vertex(G, v2) ||
      v1 == v2)
    return false;
  bool found = false;
  ALENode* edge = g->verts[v1].firstarc;
  while (edge != NULL) {
    if (edge->adjvex == v2) {
      found = true;
      break;
    }
    edge = edge->nextarc;
  }
  return found;
}
static Edge first_neighbor(const void* G, VertexId v) {
  Edge invalid_edge = {.t = -1, .h = -1, .w = INFINITY};
  if (G == NULL) return invalid_edge;
  ALGraph* g = (ALGraph*)G;
  if (v >= 0 && v < g->n_verts && g->verts[v].firstarc) {
    Edge res = {
        .t = v,
        .h = g->verts[v].firstarc->adjvex,
        .w = g->verts[v].firstarc->w,
    };
    return res;
  }
  return invalid_edge;
}
static Edge next_neighbor(const void* G, VertexId v, VertexId w) {
  Edge invalid_edge = {.t = -1, .h = -1, .w = INFINITY};
  if (G == NULL) return invalid_edge;
  ALGraph* g = (ALGraph*)G;
  if (!g->bg.iops.valid_vertex(G, w) || !g->bg.iops.valid_vertex(G, v)) {
    return invalid_edge;
  }
  ALENode* edge_v = g->verts[v].firstarc;
  while (edge_v != NULL && edge_v->adjvex != w) {
    edge_v = edge_v->nextarc;
  }
  if (edge_v == NULL || edge_v->nextarc == NULL) return invalid_edge;
  Edge res = {
      .t = v,
      .h = edge_v->nextarc->adjvex,
      .w = edge_v->nextarc->w,
  };
  return res;
}
static Edge* all_edges(const void* G, bool filter) {
  if (!G) return NULL;
  ALGraph* g = (ALGraph*)G;
  int e_count = 0;
  int n_edges = (g->directed || !filter) ? g->n_edges : g->n_edges / 2;
  Edge* edges = (Edge*)malloc(sizeof(Edge) * n_edges);
  for (VertexId i = 0; i < g->n_verts; i++) {
    ALENode* e = g->verts[i].firstarc;
    while (e != NULL) {
      if (g->directed || !filter || i < e->adjvex) {
        if (e_count < n_edges) {
          edges[e_count].t = i;
          edges[e_count].h = e->adjvex, edges[e_count].w = e->w;
          e_count++;
        }
      }
      e = e->nextarc;
    }
  }
  return edges;
}
static Weight* get_matrix(const void* G) {
  if (G == NULL) return NULL;
  ALGraph* g = (ALGraph*)G;
  Weight* mat = NULL;
  int n = g->n_verts;
  Edge* edges = all_edges(G, false);
  if (edges->h == -1) goto cleanup;
  size_t size = sizeof(Weight) * g->n_verts * g->n_verts;
  mat = (Weight*)malloc(size);
  if (mat == NULL) goto cleanup;
  memset(mat, 0, sizeof(Weight) * n * n);
  for (int i = 0; i < g->n_edges; i++) {
    VertexId h = edges[i].h;
    VertexId t = edges[i].t;
    mat[h * n + t] = edges[i].w;
  }
cleanup:
  if (edges) free(edges);
  return mat;
}

GraphQueryOps const ALGRAPH_QOPS = {
    .adjacent = adjacent,
    .first_neighbor = first_neighbor,
    .next_neighbor = next_neighbor,
    .all_edges = all_edges,
    .get_materix = get_matrix,
};

static VertexId add_vert(void* G) {
  if (G == NULL) return -1;
  ALGraph* g = (ALGraph*)G;
  if (g->n_verts >= g->vert_capacity) {
    int new_capacity = (g->vert_capacity == 0) ? 4 : g->vert_capacity * 2;
    ALVNode* temp = (ALVNode*)realloc(g->verts, sizeof(ALVNode) * new_capacity);
    if (temp == NULL) return -1;
    g->verts = temp;
    g->vert_capacity = new_capacity;
  }
  ALVNode* new_node_ptr = &(g->verts[g->n_verts]);
  new_node_ptr->data = NULL;
  new_node_ptr->firstarc = NULL;

  g->n_verts += 1;
  return g->n_verts - 1;
}
static inline void free_edges(ALGraph* g, VertexId v) {
  for (int i = 0; i < g->n_verts; i++) {
    ALENode* curr = g->verts[i].firstarc;
    ALENode* prev = NULL;

    while (curr != NULL) {
      bool should_free = false;

      if (i == v || curr->adjvex == v) {
        should_free = true;
      }

      if (should_free) {
        ALENode* tmp = curr;
        if (prev == NULL) {
          g->verts[i].firstarc = curr->nextarc;
          curr = g->verts[i].firstarc;
        } else {
          prev->nextarc = curr->nextarc;
          curr = prev->nextarc;
        }
        free(tmp);
        g->n_edges--;
      } else {
        // 如果这条边指向的顶点索引在 v 之后，且我们要进行数组前移
        // 那么这条边的目标索引必须减 1 才能指向正确的“新位置”
        if (curr->adjvex > v) {
          curr->adjvex--;
        }
        prev = curr;
        curr = curr->nextarc;
      }
    }
  }
}
static bool delete_vert(void* G, VertexId v) {
  ALGraph* g = (ALGraph*)G;
  if (!g || v < 0 || v >= g->n_verts) return false;
  free_edges(g, v);
  for (int i = v; i < g->n_verts - 1; i++) {
    g->verts[i] = g->verts[i + 1];
  }

  g->n_verts--;
  return true;
}

/*
 * adding the same edge more than one time will break the the constancy
 * between n_edges and the real edge linklist in the graph and pollibly
 * cause UNDEFINED BEAHVIOR
 */
static bool add_edge(void* G, VertexId v1, VertexId v2, Weight w) {
  if (G == NULL) return false;
  ALGraph* g = (ALGraph*)G;

  if (v1 < 0 || v1 >= g->n_verts || v2 < 0 || v2 >= g->n_verts) {
    return false;
  }
  bool duplicated = adjacent(G, v1, v2);
  assert(duplicated == false && "Duplicate edge detected!");
  ALENode* new_edge = (ALENode*)malloc(sizeof(ALENode));
  if (new_edge == NULL) return false;

  new_edge->adjvex = v2;
  new_edge->w = w;
  new_edge->nextarc = g->verts[v1].firstarc;
  g->verts[v1].firstarc = new_edge;

  g->n_edges++;
  return true;
}

static bool remove_edge(void* G, VertexId v1, VertexId v2) {
  if (G == NULL) return false;
  ALGraph* g = (ALGraph*)G;

  if (v1 < 0 || v1 >= g->n_verts || v2 < 0 || v2 >= g->n_verts) {
    return false;
  }

  ALENode* curr = g->verts[v1].firstarc;
  ALENode* prev = NULL;

  while (curr != NULL) {
    if (curr->adjvex == v2) {
      if (prev == NULL) {
        g->verts[v1].firstarc = curr->nextarc;
      } else {
        prev->nextarc = curr->nextarc;
      }

      free(curr);
      g->n_edges--;
      return true;
    }
    prev = curr;
    curr = curr->nextarc;
  }

  return false;
}
GraphMutateOps const ALGRAPH_MOPS = {
    .add_edge = add_edge,
    .remove_edge = remove_edge,
    .add_vert = add_vert,
    .delete_vert = delete_vert,
};

// 更新边权：找到边则修改并返回旧权值，没找到则返回 -1 (或定义的无效权值)
static Weight update_edge_weight(void* G, VertexId v1, VertexId v2, Weight w) {
  if (G == NULL) return -1;
  ALGraph* g = (ALGraph*)G;

  if (v1 < 0 || v1 >= g->n_verts || v2 < 0 || v2 >= g->n_verts) return -1;

  ALENode* curr = g->verts[v1].firstarc;
  while (curr != NULL) {
    if (curr->adjvex == v2) {
      Weight old_w = curr->w;
      curr->w = w;  // 更新权重
      return old_w;
    }
    curr = curr->nextarc;
  }
  return -1;  // 边不存在
}

// 获取边权：找到则返回权值，否则返回 -1
static Weight get_edge_weight(const void* G, VertexId v1, VertexId v2) {
  if (G == NULL) return -1;
  ALGraph* g = (ALGraph*)G;

  if (v1 < 0 || v1 >= g->n_verts || v2 < 0 || v2 >= g->n_verts) return -1;

  ALENode* curr = g->verts[v1].firstarc;
  while (curr != NULL) {
    if (curr->adjvex == v2) {
      return curr->w;
    }
    curr = curr->nextarc;
  }
  return -1;
}

static const WeightedGraphOps ALGRAPH_WOPS = {
    .update_edge_weight = update_edge_weight,
    .get_edge_weight = get_edge_weight};

ALGraph* algraph_init(int capccity, bool directed) {
  ALGraph* g = (ALGraph*)malloc(sizeof(ALGraph));
  if (!g) return NULL;

  BaseGraph initial_bg = {.iops = ALGRAPH_IOPS,
                          .qops = ALGRAPH_QOPS,
                          .mops = ALGRAPH_MOPS,
                          .wops = ALGRAPH_WOPS};

  g->bg = initial_bg;
  g->n_verts = 0;
  g->n_edges = 0;
  g->directed = directed;

  g->vert_capacity = (capccity > 0) ? capccity : 4;
  g->verts = (ALVNode*)malloc(g->vert_capacity * sizeof(ALVNode));

  if (!g->verts) {
    free(g);
    return NULL;
  }

  // 初始化已分配的顶点空间（防止悬空指针）
  for (int i = 0; i < g->vert_capacity; i++) {
    g->verts[i].firstarc = NULL;
    g->verts[i].data = NULL;
  }
  return g;
}

void algraph_destroy(ALGraph* g) {
  if (g == NULL) return;

  // 1. 释放所有边的链表节点
  for (int i = 0; i < g->n_verts; i++) {
    ALENode* curr = g->verts[i].firstarc;
    while (curr != NULL) {
      ALENode* temp = curr;
      curr = curr->nextarc;
      free(temp);  // 释放边节点
    }
  }

  if (g->verts != NULL) {
    free(g->verts);
  }

  free(g);
}
