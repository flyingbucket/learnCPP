#include <algorithm>
#include <catch2/catch.hpp>
#include <random>
#include <set>
#include <utility>  // std::pair
#include <vector>

#include "graph/ALGraph.h"
#include "graph/GraphAlgorithms.h"
#include "graph/MGraph.h"

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
TEST_CASE("MST Algorithm Performance", "[benchmark]") {
  // 定义不同规模的参数
  struct Case {
    std::string name;
    int nv;
    int ne;
    bool is_dense;
  };

  std::vector<Case> cases = {
      {"Small Sparse", 500, 1000, false},   // 边数 = 2V
      {"Large Sparse", 2000, 5000, false},  // 边数 = 2.5V
      {"Medium", 1000, 50000, true},        // 边数 = 50V
      {"Dense", 500, 120000, true}          // 接近完全图 (500*499/2 = 124750)
  };

  for (const auto& c : cases) {
    std::string tag = "[" + c.name + "]";

    // --- ALGraph 性能测试 ---
    SECTION("ALGraph " + c.name) {
      ALGraph* g = algraph_init(c.nv);
      g->directed = true;
      if (c.is_dense) {
        SetupDenseRandomGraph(g, c.nv, c.ne, &g->bg);
      } else {
        SetupRandomGraph(g, c.nv, c.ne, &g->bg);
      }

      BENCHMARK("Kruskal " + tag) {
        auto res = Kruskal(g, &g->bg);
        DestroyMSTResult(&res);
        return 0;
      };

      algraph_destroy(g);
    }

    // --- MGraph 性能测试 ---
    SECTION("MGraph " + c.name) {
      MGraph* g = mgraph_init(c.nv);

      g->n_verts = c.nv;

      if (c.is_dense) {
        SetupDenseRandomGraph(g, c.nv, c.ne, &g->bg);
      } else {
        SetupRandomGraph(g, c.nv, c.ne, &g->bg);
      }

      BENCHMARK("Kruskal " + tag) {
        auto res = Kruskal(g, &g->bg);
        DestroyMSTResult(&res);
        return 0;
      };

      mgraph_destroy(g);
    }
  }
}
