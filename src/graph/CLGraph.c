#include "graph/CLGraph.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph/GraphOps.h"

static VertexId add_vert(void* G) {
  if (G == NULL) return -1;
  CLGraph* g = (CLGraph*)G;
  VertexId new_id = g->n_verts;
  if (g->n_verts >= g->vert_capacity) {
    int new_capacity = (g->vert_capacity == 0) ? 4 : g->vert_capacity * 2;
    CLVNode* tmp = (CLVNode*)realloc(g->xlist, new_capacity);
    if (tmp == NULL) return -1;
    g->xlist = tmp;
    g->vert_capacity = new_capacity;
    g->xlist[new_id].data = NULL;
    g->xlist[new_id].firstin = NULL;
    g->xlist[new_id].firstout = NULL;
  }
  g->n_verts++;
  return new_id;
}
static bool add_edge(void* G, VertexId v1, VertexId v2, Weight w) {
  if (G == NULL) return -1;
  CLGraph* g = (CLGraph*)G;
  if (!g->bg.iops.valid_vertex(G, v1) || !g->bg.iops.valid_vertex(G, v2) ||
      v1 == v2)
    return false;

  CLENode* edge = (CLENode*)malloc(sizeof(CLENode));
  if (!edge) return false;

  edge->w = w;
  edge->tailvex = v1;
  edge->headvex = v2;

  edge->tlink = g->xlist[v1].firstout;
  g->xlist[v1].firstout = edge;

  edge->hlink = g->xlist[v2].firstin;
  g->xlist[v2].firstin = edge;

  g->n_edges++;
  return true;
}
static bool remove_edge(void* G, VertexId v1, VertexId v2) {
  if (G == NULL) return -1;
  CLGraph* g = (CLGraph*)G;
  if (!g->bg.iops.valid_vertex(G, v1) || !g->bg.iops.valid_vertex(G, v2) ||
      v1 == v2)
    return false;
  CLENode* p = g->xlist[v1].firstout;
  CLENode* pre = NULL;

  // 在 v1 的出边链表中找到该边并移除
  while (p && p->headvex != v2) {
    pre = p;
    p = p->tlink;
  }
  if (!p) return false;

  if (!pre)
    g->xlist[v1].firstout = p->tlink;
  else
    pre->tlink = p->tlink;

  // 在 v2 的入边链表中找到该边并移除
  CLENode* q = g->xlist[v2].firstin;
  CLENode* qpre = NULL;
  while (q && q->tailvex != v1) {
    qpre = q;
    q = q->hlink;
  }

  if (!qpre)
    g->xlist[v2].firstin = q->hlink;
  else
    qpre->hlink = q->hlink;

  free(p);
  g->n_edges--;
  return true;
}
static bool delete_vert(void* G, VertexId v) {
  if (G == NULL) return false;
  CLGraph* g = (CLGraph*)G;
  if (!g->bg.iops.valid_vertex(G, v)) return false;

  CLENode* p = g->xlist[v].firstout;
  while (p) {
    CLENode* next = p->tlink;
    remove_edge(G, v, p->headvex);
    p = next;
  }
  p = g->xlist[v].firstin;
  while (p) {
    CLENode* next = p->hlink;
    remove_edge(G, p->tailvex, v);
    p = next;
  }

  // 凡是索引大于 v 的顶点，其对应的 edge 节点字段都要减 1
  for (int i = 0; i < g->n_verts; i++) {
    if (i == v) continue;

    CLENode* edge = g->xlist[i].firstout;
    while (edge) {
      if (edge->tailvex > v) edge->tailvex--;
      if (edge->headvex > v) edge->headvex--;
      edge = edge->tlink;
    }
  }

  for (int i = v; i < g->n_verts - 1; i++) {
    g->xlist[i] = g->xlist[i + 1];
  }

  g->n_verts--;
  if (g->n_verts > 0 && 4 * g->n_verts < g->vert_capacity &&
      g->vert_capacity > 8) {
    int new_capacity = g->vert_capacity >> 1;
    CLVNode* tmp = (CLVNode*)realloc(g->xlist, sizeof(CLVNode) * new_capacity);
    if (tmp) {
      g->xlist = tmp;
      g->vert_capacity = new_capacity;
    }
  }

  return true;
}
GraphMutateOps CLGRAPH_MOPS = {.add_vert = add_vert,
                               .delete_vert = delete_vert,
                               .add_edge = add_edge,
                               .remove_edge = remove_edge};

static int vertex_count(void* G) {
  if (G == NULL) return 0;
  CLGraph* g = (CLGraph*)G;
  return g->n_verts;
}
static int edge_count(void* G) {
  if (G == NULL) return 0;
  CLGraph* g = (CLGraph*)G;
  return g->n_edges;
}
static bool valid_vertex(void* G, VertexId v) {
  if (G == NULL) return false;
  CLGraph* g = (CLGraph*)G;
  return v >= 0 && v < g->n_verts;
}
static bool directed(void* G) {
  printf("Error:Not implemented");
  (void)G;
  return false;
}
GraphInfoOps CLGRAPH_IOPS = {
    .directed = directed,
    .edge_count = edge_count,
    .valid_vertex = valid_vertex,
    .vertex_count = vertex_count,
};

/**
 * 判断是否存在从 v1 到 v2 的边
 * 效率：O(out_degree(v1))
 */
static bool adjacent(void* G, VertexId v1, VertexId v2) {
  CLGraph* g = (CLGraph*)G;
  if (!valid_vertex(G, v1) || !valid_vertex(G, v2) || v1 == v2) return false;

  CLENode* p = g->xlist[v1].firstout;
  while (p) {
    if (p->headvex == v2) return true;
    p = p->tlink;
  }
  return false;
}

/**
 * 找到顶点 v 的第一个邻接点
 * 效率：O(1) 找到边，O(1) 返回 ID
 */
static Edge first_neighbor(void* G, VertexId v) {
  Edge invalid_edge = {.t = -1, .h = -1, .w = INFINITY};
  CLGraph* g = (CLGraph*)G;
  if (!valid_vertex(G, v)) return invalid_edge;

  CLENode* p = g->xlist[v].firstout;
  if (p) {
    VertexId h = (VertexId)p->headvex;
    Edge res = {
        .t = v,
        .h = h,
        .w = p->w,
    };
    return res;
  }
  return invalid_edge;
}

/**
 * 找到顶点 v 相对于邻接点 w 的下一个邻接点
 * 效率：O(out_degree(v))
 */
static Edge next_neighbor(void* G, VertexId v, VertexId w) {
  Edge invalid_edge = {.t = -1, .h = -1, .w = INFINITY};
  if (G == NULL) return invalid_edge;
  CLGraph* g = (CLGraph*)G;
  if (!valid_vertex(G, v) || !valid_vertex(G, w) || v == w) return invalid_edge;

  CLENode* p = g->xlist[v].firstout;

  while (p && p->headvex != w) {
    p = p->tlink;
  }

  if (p && p->tlink) {
    VertexId h = (VertexId)p->tlink->headvex;
    Edge res = {
        .t = v,
        .h = h,
        .w = p->w,
    };
    return res;
  }

  return invalid_edge;
}

GraphQueryOps CLGRAPH_QOPS = {.adjacent = adjacent,
                              .first_neighbor = first_neighbor,
                              .next_neighbor = next_neighbor};

static Weight get_edge_weight(void* G, VertexId v1, VertexId v2) {
  if (G == NULL) return -1;
  CLGraph* g = (CLGraph*)G;
  if (!valid_vertex(G, v1) || !valid_vertex(G, v2) || v1 == v2) return -1;

  CLENode* p = g->xlist[v1].firstout;
  while (p) {
    if (p->headvex == v2) {
      return p->w;
    }
    p = p->tlink;
  }
  return -1;
}

static Weight update_edge_weight(void* G, VertexId v1, VertexId v2, Weight w) {
  if (G == NULL) return -1;
  CLGraph* g = (CLGraph*)G;
  if (!valid_vertex(G, v1) || !valid_vertex(G, v2) || v1 == v2) return -1;

  CLENode* p = g->xlist[v1].firstout;
  while (p) {
    if (p->headvex == v2) {
      Weight old_w = p->w;
      p->w = w;
      return old_w;
    }
    p = p->tlink;
  }
  return -1;
}

WeightedGraphOps CLGRAPH_WOPS = {.get_edge_weight = get_edge_weight,
                                 .update_edge_weight = update_edge_weight};

CLGraph* clgraph_init(int capacity) {
  CLGraph* g = (CLGraph*)malloc(sizeof(CLGraph));
  if (!g) return NULL;

  g->n_verts = 0;
  g->n_edges = 0;  // 初始边数为 0
  g->vert_capacity = (capacity > 8) ? capacity : 8;

  g->xlist = (CLVNode*)malloc(sizeof(CLVNode) * g->vert_capacity);
  if (!g->xlist) {
    free(g);
    return NULL;
  }

  // 初始化顶点数组
  for (int i = 0; i < g->vert_capacity; i++) {
    g->xlist[i].data = NULL;
    g->xlist[i].firstin = NULL;
    g->xlist[i].firstout = NULL;
  }
  g->bg.iops = CLGRAPH_IOPS;
  g->bg.qops = CLGRAPH_QOPS;
  g->bg.mops = CLGRAPH_MOPS;
  g->bg.wops = CLGRAPH_WOPS;

  return g;
}

void destroy_clgraph(CLGraph* g) {
  if (!g) return;

  // 遍历每个顶点，释放其所有的出边节点
  for (int i = 0; i < g->n_verts; i++) {
    CLENode* p = g->xlist[i].firstout;
    while (p) {
      CLENode* temp = p;
      p = p->tlink;
      free(temp);
    }
  }

  // 释放顶点数组和图结构体
  free(g->xlist);
  free(g);
}
