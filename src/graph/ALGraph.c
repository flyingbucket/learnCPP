#include "graph/ALGraph.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graph/GraphOps.h"

static int vertex_count(void* G) {
  if (G == NULL) return -1;
  ALGraph* g = (ALGraph*)G;
  return g->n_vert;
}
static bool valid_vertex(void* G, VertexId v) {
  if (G == NULL) return false;
  ALGraph* g = (ALGraph*)G;
  for (int i = 0; i < g->n_vert; i++) {
    int id = g->verts[i].id;
    if (i == id && v == id) return true;
  }
  return false;
}
static bool directed(void* G) {
  printf("Error: Not Implemented");
  (void)G;
  return false;
}

GraphInfoOps const ALGRAPH_IOPS = {
    .vertex_count = vertex_count,
    .valid_vertex = valid_vertex,
    .directed = directed,
};

static bool adjacent(void* G, VertexId v1, VertexId v2) {
  if (G == NULL) return false;
  ALGraph* g = (ALGraph*)G;
  if (!g->bg->iops->valid_vertex(G, v1) || !g->bg->iops->valid_vertex(G, v2) ||
      v1 == v2)
    return false;
  bool found = false;
  ENode* edge = g->verts->firstarc;
  while (edge != NULL) {
    if (edge->adjvex == v2) {
      found = true;
      break;
    }
    edge = edge->nextarc;
  }
  return found;
}
static int first_neighbor(void* G, VertexId v) {
  if (G == NULL) return NULL;
  ALGraph* g = (ALGraph*)G;
  for (int i = 0; i < g->n_vert; i++) {
    if (g->verts[i].id == v) {
      return g->verts[i].firstarc->adjvex;
    }
  }
  return -1;
}
static int next_neighbor(void* G, VertexId v, VertexId w) {
  if (G == NULL) return -1;
  ALGraph* g = (ALGraph*)G;
  if (!g->bg->iops->valid_vertex(G, v) || !g->bg->iops->valid_vertex(G, w)) {
    return -1;
  }
  ENode* edge_v = g->verts[v].firstarc;
  while (edge_v != NULL && edge_v->adjvex != w) {
    edge_v = edge_v->nextarc;
  }
  if (edge_v == NULL) return -1;
  return edge_v->nextarc->adjvex;
}
GraphQueryOps const ALGRAPH_QOPS = {
    .adjacent = adjacent,
    .first_neighbor = first_neighbor,
    .next_neighbor = next_neighbor,
};

static VertexId add_vert(void* G) {
  if (G == NULL) return -1;
  ALGraph* g = (ALGraph*)G;
  if (g->n_vert >= g->vert_capacity) {
    int new_capacity = (g->vert_capacity == 0) ? 4 : g->vert_capacity * 2;
    VNode* temp = (VNode*)realloc(g->verts, sizeof(VNode) * new_capacity);
    if (temp == NULL) return -1;
    g->verts = temp;
    g->vert_capacity = new_capacity;
  }

  VNode* new_node_ptr = &(g->verts[g->n_vert]);
  new_node_ptr->data = NULL;
  new_node_ptr->id = g->n_vert;
  new_node_ptr->firstarc = NULL;

  VertexId new_id = new_node_ptr->id;
  g->n_vert += 1;
  return new_id;
}
static inline void free_edges(ALGraph* g, VertexId v) {
  for (int i = 0; i < g->n_vert; i++) {
    ENode* curr = g->verts[i].firstarc;
    ENode* prev = NULL;

    while (curr != NULL) {
      bool should_free = false;

      // 情况1：当前顶点就是被删顶点，释放它发出的所有边
      if (i == v) {
        should_free = true;
      }
      // 情况2：这条边指向被删顶点
      else if (curr->adjvex == v) {
        should_free = true;
      }

      if (should_free) {
        ENode* tmp = curr;
        if (prev == NULL) {
          g->verts[i].firstarc = curr->nextarc;
          curr = g->verts[i].firstarc;
        } else {
          prev->nextarc = curr->nextarc;
          curr = prev->nextarc;
        }
        free(tmp);
        g->n_edge--;
      } else {
        // 关键修正：如果这条边指向的顶点索引在 v 之后，且我们要进行数组前移
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
  if (!g || v < 0 || v >= g->n_vert) return false;
  free_edges(g, v);
  for (int i = v; i < g->n_vert - 1; i++) {
    g->verts[i] = g->verts[i + 1];
    g->verts[i].id = i;
  }

  g->n_vert--;
  return true;
}
static bool add_edge(void* G, VertexId v1, VertexId v2, Weight w) {
  if (G == NULL) return false;
  ALGraph* g = (ALGraph*)G;

  if (v1 < 0 || v1 >= g->n_vert || v2 < 0 || v2 >= g->n_vert) {
    return false;
  }

  ENode* new_edge = (ENode*)malloc(sizeof(ENode));
  if (new_edge == NULL) return false;

  new_edge->adjvex = v2;
  new_edge->w = w;
  new_edge->nextarc = g->verts[v1].firstarc;
  g->verts[v1].firstarc = new_edge;

  g->n_edge++;
  return true;
}

static bool remove_edge(void* G, VertexId v1, VertexId v2) {
  if (G == NULL) return false;
  ALGraph* g = (ALGraph*)G;

  if (v1 < 0 || v1 >= g->n_vert || v2 < 0 || v2 >= g->n_vert) {
    return false;
  }

  ENode* curr = g->verts[v1].firstarc;
  ENode* prev = NULL;

  while (curr != NULL) {
    if (curr->adjvex == v2) {
      if (prev == NULL) {
        g->verts[v1].firstarc = curr->nextarc;
      } else {
        prev->nextarc = curr->nextarc;
      }

      free(curr);
      g->n_edge--;
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

  if (v1 < 0 || v1 >= g->n_vert || v2 < 0 || v2 >= g->n_vert) return -1;

  ENode* curr = g->verts[v1].firstarc;
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
static Weight get_edge_weight(void* G, VertexId v1, VertexId v2) {
  if (G == NULL) return -1;
  ALGraph* g = (ALGraph*)G;

  if (v1 < 0 || v1 >= g->n_vert || v2 < 0 || v2 >= g->n_vert) return -1;

  ENode* curr = g->verts[v1].firstarc;
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

ALGraph* algraph_create(int n_vert, int n_edge) {
  // 1. 分配 ALGraph 结构体内存
  ALGraph* g = (ALGraph*)malloc(sizeof(ALGraph));
  if (!g) return NULL;

  // 2. 分配 BaseGraph (Trait 容器) 内存并绑定静态 Ops
  g->bg = (BaseGraph*)malloc(sizeof(BaseGraph));
  if (!g->bg) {
    free(g);
    return NULL;
  }
  g->bg->iops = &ALGRAPH_IOPS;
  g->bg->qops = &ALGRAPH_QOPS;
  g->bg->mops = &ALGRAPH_MOPS;
  g->bg->wops = &ALGRAPH_WOPS;

  // 3. 初始化基本计数
  g->n_vert = 0;
  g->n_edge = 0;

  // 4. 预分配顶点数组空间
  // 如果传入 n_vert > 0，则按需分配；否则默认为 0，等待 add_vert 触发 realloc
  g->vert_capacity = (n_vert > 0) ? n_vert : 4;
  g->verts = (VNode*)malloc(g->vert_capacity * sizeof(VNode));

  if (!g->verts) {
    free(g->bg);
    free(g);
    return NULL;
  }

  // 初始化已分配的顶点空间（防止悬空指针）
  for (int i = 0; i < g->vert_capacity; i++) {
    g->verts[i].firstarc = NULL;
    g->verts[i].data = NULL;
    g->verts[i].id = -1;
  }
  (void)n_edge;
  return g;
}

void algraph_destroy(ALGraph* g) {
  if (g == NULL) return;

  // 1. 释放所有边的链表节点
  for (int i = 0; i < g->n_vert; i++) {
    ENode* curr = g->verts[i].firstarc;
    while (curr != NULL) {
      ENode* temp = curr;
      curr = curr->nextarc;
      free(temp);  // 释放边节点
    }
  }

  // 2. 释放顶点数组
  if (g->verts != NULL) {
    free(g->verts);
  }

  // 3. 释放 Trait 容器
  if (g->bg != NULL) {
    free(g->bg);
  }

  // 4. 最后释放图对象本身
  free(g);
}
