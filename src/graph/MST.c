#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "graph/GraphAlgorithms.h"
#include "graph/GraphOps.h"
#include "queue/SqQueue.h"
#include "tree/UFset.h"

MSTResult* Prim(const void* graph_context, const BaseGraph* graph_methods) {
  if (!graph_context || !graph_methods) return NULL;

  const void* G = graph_context;
  const BaseGraph* bg = graph_methods;

  // Prim can only deal with undirected graph!
  if (bg->iops.directed(G)) return NULL;

  int nv = bg->iops.vertex_count(G);
  MSTResult* result = (MSTResult*)malloc(sizeof(MSTResult));
  result->component_count = 0;
  result->edge_count = 0;
  result->edges = (Edge*)malloc(sizeof(Edge) * (nv - 1));
  result->component_offset = NULL;
  if (result->edges == NULL) {
    free(result);
    return NULL;
  }
  SqQueue* offset;
  bool ok = InitQueue(&offset, sizeof(VertexId), nv - 1);
  if (!ok) return NULL;

  bool* in_tree = (bool*)calloc(nv, sizeof(bool));
  Weight* dist_to_tree = (Weight*)malloc(sizeof(Weight) * nv);
  VertexId* parent = (VertexId*)malloc(sizeof(VertexId) * nv);
  for (int i = 0; i < nv; i++) {
    dist_to_tree[i] = INFINITY;
    parent[i] = -1;
  }
  int edge_count = 0;

  // traverse all components
  for (int start_v = 0; start_v < nv; start_v++) {
    if (in_tree[start_v]) continue;

    dist_to_tree[start_v] = 0;
    // build MST for one component
    for (int _ = 0; _ < nv; _++) {
      VertexId target = -1;
      Weight min_w = INFINITY;
      for (VertexId i = 0; i < nv; i++) {
        if (!in_tree[i] && dist_to_tree[i] < min_w) {
          min_w = dist_to_tree[i];
          target = i;
        }
      }

      if (target == -1)
        break;  // no targeted vex found outside tree, which means the MST of
                // this component is complete

      in_tree[target] = true;
      if (parent[target] !=
          -1) {  // not the  first vex of this component, add edge
        result->edges[edge_count++] =
            (Edge){.t = parent[target], .h = target, .w = min_w};
      }

      // update recentlly added target's neighbors' dist_to_tree
      Edge neighbor = bg->qops.first_neighbor(G, target);
      while (neighbor.h >= 0) {
        Weight old = dist_to_tree[neighbor.h];
        if (!in_tree[neighbor.h] && old > neighbor.w) {
          dist_to_tree[neighbor.h] = neighbor.w;
          parent[neighbor.h] = target;
        }
        neighbor = bg->qops.next_neighbor(G, target, neighbor.h);
      }
    }

    result->component_count++;
    EnQueue(offset, &edge_count);
  }

  Edge* tmp = (Edge*)realloc(result->edges, sizeof(Edge) * edge_count);
  if (tmp != NULL) result->edges = tmp;
  result->edge_count = edge_count;
  // 分配 n_comp + 1 个空间，最后一个放总边数作为“哨兵”
  result->component_offset =
      (int*)malloc((result->component_count + 1) * sizeof(int));

  for (int i = 0; i < result->component_count; i++) {
    DeQueue(offset, &result->component_offset[i]);
  }
  result->component_offset[result->component_count] = edge_count;  // 哨兵
  assert(isSqQueueEmpty(offset));
  assert(result->edge_count == nv - result->component_count);
  DestorySqQueue(&offset);
  free(in_tree);
  free(dist_to_tree);
  free(parent);
  return result;
}

static inline int compare_edge(const void* e1, const void* e2) {
  Edge* l = (Edge*)e1;
  Edge* r = (Edge*)e2;

  if (l->w < r->w) return -1;
  if (l->w > r->w) return 1;
  return 0;  // 必须处理相等的情况！
}
MSTResult* Kruskal(const void* graph_context, const BaseGraph* bg) {
  if (!graph_context || !bg) return NULL;

  Edge* all_edges = NULL;
  UFset set = NULL;
  Edge* mst_edges = NULL;
  Edge* arranged_edges = NULL;
  int* offset = NULL;
  MSTResult* result = NULL;

  // 计数排序的临时数组
  int* root_edge_count = NULL;
  int* start_pos = NULL;

  const void* G = graph_context;
  int nv = bg->iops.vertex_count(G);
  int ne = (bg->iops.directed(G)) ? bg->iops.edge_count(G)
                                  : bg->iops.edge_count(G) / 2;
  all_edges = bg->qops.all_edges(G);
  if (!all_edges) goto cleanup;
  qsort(all_edges, ne, sizeof(Edge), compare_edge);
  for (int i = 0; i < ne; i++) {
    if (all_edges[i].t < 0 || all_edges[i].t >= nv || all_edges[i].h < 0 ||
        all_edges[i].h >= nv) {
      printf("\n[ERROR] Corrupted edge at index %d: t=%d, h=%d, ne=%d\n", i,
             all_edges[i].t, all_edges[i].h, ne);
      goto cleanup;
    }
  }
  if (!InitUFset(&set, nv)) goto cleanup;

  mst_edges = (Edge*)malloc(sizeof(Edge) * (nv - 1));
  if (!mst_edges) goto cleanup;

  // Kruskal
  int e_count = 0;
  for (int i = 0; i < ne; i++) {
    VertexId r1 = Find(set, all_edges[i].t);
    VertexId r2 = Find(set, all_edges[i].h);
    if (r1 != r2) {
      Union(set, r1, r2);
      mst_edges[e_count] = all_edges[i];
      e_count++;
    }
  }

  if (e_count == 0) goto cleanup;

  // 计数排序边重排: O(V + E)
  int n_comp = nv - e_count;

  offset = (int*)malloc(sizeof(int) * (n_comp + 1));
  arranged_edges = (Edge*)malloc(sizeof(Edge) * e_count);
  root_edge_count = (int*)calloc(nv, sizeof(int));
  start_pos = (int*)calloc(nv, sizeof(int));

  if (!offset || !arranged_edges || !root_edge_count || !start_pos)
    goto cleanup;

  // 统计每个连通分量（根节点）包含的边数
  for (int i = 0; i < e_count; i++) {
    int r = Find(set, mst_edges[i].t);
    root_edge_count[r]++;
  }

  // 计算偏移量（前缀和）
  int current_offset = 0;
  int comp_idx = 0;
  for (int i = 0; i < nv; i++) {
    if (set[i] < 0) {  // 这是一个根节点,代表一个连通分量
      offset[comp_idx] = current_offset;
      start_pos[i] = current_offset;
      current_offset += root_edge_count[i];
      comp_idx++;
    }
  }
  offset[n_comp] = e_count;
  // 确保存下的分量数和理论值一致
  if (comp_idx != n_comp) goto cleanup;

  // 将边填入正确位置
  for (int i = 0; i < e_count; i++) {
    int r = Find(set, mst_edges[i].t);
    int target_idx = start_pos[r];
    arranged_edges[target_idx] = mst_edges[i];
    start_pos[r]++;  // 写入后，该分量的指针后移一位
  }

  result = (MSTResult*)malloc(sizeof(MSTResult));
  if (!result) goto cleanup;

  result->component_count = n_comp;
  result->edge_count = e_count;
  result->edges = arranged_edges;
  result->component_offset = offset;

  // 所有权转移
  arranged_edges = NULL;
  offset = NULL;

cleanup:
  if (all_edges) free(all_edges);
  if (set) DestroyUFset(&set);
  if (mst_edges) free(mst_edges);

  // 清理计数排序的临时数组
  if (root_edge_count) free(root_edge_count);
  if (start_pos) free(start_pos);

  // 如果失败或中途报错，下面这两个指针不会是 NULL，就会被正确释放
  // 如果成功，它们在上面已经被置为 NULL，free(NULL) 是安全的
  if (arranged_edges) free(arranged_edges);
  if (offset) free(offset);

  return result;  // 如果失败，result 会是最初始的 NULL
}
