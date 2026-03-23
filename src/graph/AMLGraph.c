#include "graph/AMLGraph.h"

#include <stdbool.h>
#include <stdlib.h>

#define GET_AMLG(G) ((AMLGraph*)(G))

static int vertex_count(void* G) {
  if (G == NULL) return -1;
  return GET_AMLG(G)->n_verts;
}

static bool valid_vertex(void* G, VertexId v) {
  if (G == NULL) return -1;
  AMLGraph* g = GET_AMLG(G);
  return v >= 0 && v < g->n_verts;
}

static bool is_directed(void* G) {
  (void)G;
  return false;  // 邻接多重表固定表示无向图
}

static bool adjacent(void* G, VertexId v1, VertexId v2) {
  if (G == NULL) return false;
  AMLGraph* g = GET_AMLG(G);
  if (!valid_vertex(G, v1) || !valid_vertex(G, v2) || v1 == v2) return false;

  ENode* p = g->verts[v1].firstedge;
  while (p) {
    if ((p->ivex == v1 && p->jvex == v2) || (p->ivex == v2 && p->jvex == v1))
      return true;
    p = (p->ivex == v1) ? p->ilink : p->jlink;
  }
  return false;
}

static VertexId first_neighbor(void* G, VertexId v) {
  AMLGraph* g = GET_AMLG(G);
  if (!valid_vertex(G, v) || !g->verts[v].firstedge) return -1;

  ENode* e = g->verts[v].firstedge;
  return (e->ivex == v) ? e->jvex : e->ivex;
}

static VertexId next_neighbor(void* G, VertexId v, VertexId w) {
  AMLGraph* g = GET_AMLG(G);
  if (!valid_vertex(G, v)) return -1;

  ENode* p = g->verts[v].firstedge;
  while (p) {
    VertexId curr_neighbor = (p->ivex == v) ? p->jvex : p->ivex;
    if (curr_neighbor == w) {
      ENode* next = (p->ivex == v) ? p->ilink : p->jlink;
      if (!next) return -1;
      return (next->ivex == v) ? next->jvex : next->ivex;
    }
    p = (p->ivex == v) ? p->ilink : p->jlink;
  }
  return -1;
}

static VertexId add_vert(void* G) {
  AMLGraph* g = GET_AMLG(G);
  if (g->n_verts >= g->vert_capacity) {
    // 简单的扩容逻辑
    int new_cap = g->vert_capacity * 2;
    VNode* tmp = (VNode*)realloc(g->verts, sizeof(VNode) * new_cap);
    if (!tmp) return -1;
    g->verts = tmp;
    g->vert_capacity = new_cap;
  }
  VertexId new_id = g->n_verts++;
  g->verts[new_id].data = NULL;
  g->verts[new_id].firstedge = NULL;
  return new_id;
}

static bool add_edge(void* G, VertexId v1, VertexId v2, Weight w) {
  AMLGraph* g = GET_AMLG(G);
  if (!valid_vertex(G, v1) || !valid_vertex(G, v2) || v1 == v2) return false;
  if (adjacent(G, v1, v2)) return false;  // 避免重复边

  ENode* e = (ENode*)malloc(sizeof(ENode));
  if (!e) return false;

  e->ivex = v1;
  e->jvex = v2;
  e->w = w;

  // 头插法接入 v1 的链表
  e->ilink = g->verts[v1].firstedge;
  g->verts[v1].firstedge = e;

  // 头插法接入 v2 的链表
  e->jlink = g->verts[v2].firstedge;
  g->verts[v2].firstedge = e;

  g->n_edges++;
  return true;
}

static bool remove_edge(void* G, VertexId v1, VertexId v2) {
  AMLGraph* g = GET_AMLG(G);
  if (!valid_vertex(G, v1) || !valid_vertex(G, v2)) return false;

  ENode* p = g->verts[v1].firstedge;
  // 1. 寻找该边指针
  while (p && !((p->ivex == v1 && p->jvex == v2) ||
                (p->ivex == v2 && p->jvex == v1))) {
    p = (p->ivex == v1) ? p->ilink : p->jlink;
  }
  if (!p) return false;

  // 2. 从两个顶点的链表中解开该边
  VertexId vers[2] = {v1, v2};
  for (int i = 0; i < 2; i++) {
    VertexId currV = vers[i];
    ENode** currPtr = &(g->verts[currV].firstedge);

    while (*currPtr != p) {
      if ((*currPtr)->ivex == currV)
        currPtr = &((*currPtr)->ilink);
      else
        currPtr = &((*currPtr)->jlink);
    }

    // 执行断开：将前驱指向 p 的后继
    *currPtr = (p->ivex == currV) ? p->ilink : p->jlink;
  }

  free(p);
  g->n_edges--;
  return true;
}

static Weight get_edge_weight(void* G, VertexId v1, VertexId v2) {
  AMLGraph* g = GET_AMLG(G);
  ENode* p = g->verts[v1].firstedge;
  while (p) {
    if ((p->ivex == v1 && p->jvex == v2) || (p->ivex == v2 && p->jvex == v1))
      return p->w;
    p = (p->ivex == v1) ? p->ilink : p->jlink;
  }
  return 0.0;
}

static Weight update_edge_weight(void* G, VertexId v1, VertexId v2, Weight w) {
  AMLGraph* g = GET_AMLG(G);
  ENode* p = g->verts[v1].firstedge;
  while (p) {
    if ((p->ivex == v1 && p->jvex == v2) || (p->ivex == v2 && p->jvex == v1)) {
      Weight old = p->w;
      p->w = w;
      return old;
    }
    p = (p->ivex == v1) ? p->ilink : p->jlink;
  }
  return 0.0;
}

// --- 接口表挂载 ---

static const GraphInfoOps AML_Iops = {.vertex_count = vertex_count,
                                      .valid_vertex = valid_vertex,
                                      .directed = is_directed};

static const GraphQueryOps AML_Qops = {.adjacent = adjacent,
                                       .first_neighbor = first_neighbor,
                                       .next_neighbor = next_neighbor};

static const GraphMutateOps AML_Mops = {
    .add_vert = add_vert,
    .delete_vert = NULL,  // 邻接多重表删除顶点逻辑较复杂，按需实现
    .add_edge = add_edge,
    .remove_edge = remove_edge};

static const WeightedGraphOps AML_Wops = {
    .get_edge_weight = get_edge_weight,
    .update_edge_weight = update_edge_weight};

const BaseGraph AML_INTERFACE = {
    .iops = &AML_Iops, .qops = &AML_Qops, .mops = &AML_Mops, .wops = &AML_Wops};

// --- 生命周期管理 ---

AMLGraph* amlgraph_create(int n_verts) {
  AMLGraph* g = (AMLGraph*)malloc(sizeof(AMLGraph));
  if (!g) return NULL;

  g->bg = AML_INTERFACE;
  g->n_verts = 0;
  g->n_edges = 0;
  g->vert_capacity = (n_verts > 8) ? n_verts : 8;
  g->verts = (VNode*)malloc(sizeof(VNode) * g->vert_capacity);

  for (int i = 0; i < g->vert_capacity; i++) {
    g->verts[i].data = NULL;
    g->verts[i].firstedge = NULL;
  }
  return g;
}

void amlgraph_destroy(AMLGraph* g) {
  if (!g) return;
  for (int i = 0; i < g->n_verts; i++) {
    ENode* p = g->verts[i].firstedge;
    while (p) {
      ENode* next = (p->ivex == i) ? p->ilink : p->jlink;
      // 策略：只在遇到 ivex 端时释放内存，保证全局只释放一次
      if (p->ivex == i) {
        free(p);
      }
      p = next;
    }
  }
  free(g->verts);
  free(g);
}
