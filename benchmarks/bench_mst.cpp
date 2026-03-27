#include <catch2/catch.hpp>
#include <random>

#include "graph/ALGraph.h"
#include "graph/GraphAlgorithms.h"
#include "graph/MGraph.h"

// 辅助函数：生成随机图
template <typename GType>
void SetupRandomGraph(GType* g, int nv, int ne, BaseGraph* bg) {
  for (int i = 0; i < nv; ++i) bg->mops.add_vert(g);

  std::mt19937 rng(42);
  std::uniform_int_distribution<int> dist(0, nv - 1);
  std::uniform_real_distribution<double> weight_dist(1.0, 10.0);

  for (int i = 0; i < ne; ++i) {
    bg->mops.add_edge(g, dist(rng), dist(rng), weight_dist(rng));
  }
}

TEST_CASE("MST Benchmark", "[benchmark]") {
  int nv = 2000;
  int ne = 1000;  // 相对稀疏的图
  printf("Num vertex : %d\n", nv);
  printf("Num edges: %d\n", ne);

  SECTION("Adjacency List (ALGraph)") {
    ALGraph* g = algraph_init(nv);
    SetupRandomGraph(g, nv, ne, &g->bg);
    g->directed = true;

    BENCHMARK("Kruskal") {
      auto res = Kruskal(g, &g->bg);
      DestroyMSTResult(&res);
      return 0;
    };

    algraph_destroy(g);
  }
  // 在 bench_mst.cpp 中
  SECTION("Matrix (MGraph)") {
    MGraph* g = mgraph_init(nv);

    // 不要调用 add_vert，因为 MGraph 初始化时已经确定了大小
    // 直接手动设置节点计数（如果你的 add_edge 依赖 n_verts）
    g->n_verts = nv;

    // 只添加边
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(0, nv - 1);
    std::uniform_real_distribution<double> weight_dist(1.0, 10.0);
    for (int i = 0; i < 1000; ++i) {
      g->bg.mops.add_edge(g, dist(rng), dist(rng), weight_dist(rng));
    }

    BENCHMARK("Kruskal") {
      auto res = Kruskal(g, &g->bg);
      DestroyMSTResult(&res);
      return 0;
    };
    mgraph_destroy(g);
  }
}
