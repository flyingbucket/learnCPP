#include <unistd.h>

#include <algorithm>
#include <random>
#include <set>
#include <utility>  // std::pair
#include <vector>

#include "graph/ALGraph.h"
#include "graph/GraphAlgorithms.h"

template <typename GType>
void SetupRandomGraph(GType* g, int nv, int ne, BaseGraph* bg) {
  // 1. 添加顶点
  for (int i = 0; i < nv; ++i) {
    bg->mops.add_vert(g);
  }

  // 安全检查：防止请求的边数超过完全图的最大边数 (无向图 nv*(nv-1)/2)
  // 如果是无向图，max_edges = nv * (nv - 1) / 2;
  // 如果是有向图，max_edges = nv * (nv - 1);
  long long max_edges = (long long)nv * (nv - 1) / 2;
  if (ne > max_edges) ne = max_edges;

  std::mt19937 rng(42);
  std::uniform_int_distribution<int> dist(0, nv - 1);
  std::uniform_real_distribution<double> weight_dist(1.0, 10.0);

  // 2. 使用 set 记录已生成的边 (存储较小的顶点在前，保证无向图唯一性)
  std::set<std::pair<int, int>> seen;
  int edges_added = 0;

  while (edges_added < ne) {
    int u = dist(rng);
    int v = dist(rng);

    // 排除自环
    if (u == v) continue;

    // 保证 u < v，这样 (0,1) 和 (1,0) 会被视为同一条边（针对无向图）
    if (u > v) std::swap(u, v);

    // 如果这条边没见过，则添加
    if (seen.find({u, v}) == seen.end()) {
      bg->mops.add_edge(g, u, v, weight_dist(rng));
      seen.insert({u, v});
      edges_added++;
    }
  }
}
template <typename GType>
void SetupDenseRandomGraph(GType* g, int nv, int ne, BaseGraph* bg) {
  for (int i = 0; i < nv; ++i) bg->mops.add_vert(g);

  // 1. 生成所有可能的边
  std::vector<std::pair<int, int>> all_possible_edges;
  all_possible_edges.reserve(nv * (nv - 1) / 2);
  for (int i = 0; i < nv; ++i) {
    for (int j = i + 1; j < nv; ++j) {
      all_possible_edges.push_back({i, j});
    }
  }

  // 2. 打乱顺序
  std::mt19937 rng(42);
  std::shuffle(all_possible_edges.begin(), all_possible_edges.end(), rng);

  // 3. 取前 ne 个添加
  std::uniform_real_distribution<double> weight_dist(1.0, 10.0);
  int actual_ne = std::min((int)all_possible_edges.size(), ne);
  for (int i = 0; i < actual_ne; ++i) {
    bg->mops.add_edge(g, all_possible_edges[i].first,
                      all_possible_edges[i].second, weight_dist(rng));
  }
}

int main() {
  int nv = 2000, ne = 1000;
  printf("Before algraph_init\n");
  sleep(1);
  ALGraph* g = algraph_init(nv);
  g->directed = true;

  printf("Initializing Graph: V=%d, E=%d...\n", nv, ne);
  sleep(1);
  SetupRandomGraph(g, nv, ne, &g->bg);
  printf("Initializing Graph: V=%d, E=%d...\n", g->n_verts, g->n_edges);

  printf("Running Kruskal...\n");
  sleep(1);
  MSTResult* res = Kruskal(g, &g->bg);

  if (res) {
    printf("Kruskal Finished. MST edge count: %.d\n", res->edge_count);
    DestroyMSTResult(&res);
  } else {
    printf("Kruskal failed to find an MST (Graph might be disconnected).\n");
  }

  algraph_destroy(g);
  printf("Done.\n");
  return 0;
}
