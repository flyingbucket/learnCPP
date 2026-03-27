#include <catch2/catch.hpp>
#include <cmath>
#include <cstdio>
#include <vector>

#include "graph/ALGraph.h"
#include "graph/GraphAlgorithms.h"
#include "graph/MGraph.h"

/**
 * @brief 构造测试图的辅助模板
 */
template <typename T>
T* BuildTestGraph(int nv, const std::vector<Edge>& edges,
                  T* (*init)(int),        // 明确 init 返回 T*
                  void (*destroy)(T*)) {  // 明确 destroy 接收 T*
  T* g = init(nv);
  if (!g) return nullptr;

  // 因为你已经把 bg 放在了第一位，这里强转是安全的
  BaseGraph* bg = (BaseGraph*)g;

  for (int i = 0; i < nv; ++i) {
    if (bg->mops.add_vert) bg->mops.add_vert(g);
  }

  for (const auto& e : edges) {
    if (bg->mops.add_edge) bg->mops.add_edge(g, e.t, e.h, e.w);
  }
  return g;
}

/**
 * @brief 计算生成森林的总权重
 */
double GetTotalWeight(const MSTResult* res) {
  double sum = 0;
  for (int i = 0; i < res->edge_count; ++i) {
    sum += res->edges[i].w;
  }
  return sum;
}

TEST_CASE("MST Algorithms: Adjacency List (ALGraph)", "[ALGraph][MST]") {
  SECTION("Connected Graph: Classic Triangle") {
    // 0-1 (1.0), 1-2 (2.0), 0-2 (0.5)
    // 预期 MST: 0-1 (1.0) 和 0-2 (0.5), 总权重 1.5
    std::vector<Edge> edges = {{0, 1, 1.0}, {1, 2, 2.0}, {0, 2, 0.5}};
    ALGraph* g = BuildTestGraph(3, edges, algraph_init, algraph_destroy);
    printf("Graph built\n");

    printf("Running Kruskal\n");
    MSTResult* res_k = Kruskal(g, &g->bg);
    printf("Running Prim\n");
    MSTResult* res_p = Prim(g, &g->bg);

    REQUIRE(res_k != nullptr);
    REQUIRE(res_p != nullptr);

    // 1. 验证连通分量数量
    CHECK(res_k->component_count == 1);
    CHECK(res_p->component_count == 1);

    // 2. 验证最小总权重
    CHECK(GetTotalWeight(res_k) == Approx(1.5));
    CHECK(GetTotalWeight(res_p) == Approx(1.5));

    // 3. 验证哨兵位逻辑 (n_comp + 1)
    CHECK(res_k->component_offset[0] == 0);
    CHECK(res_k->component_offset[1] == 2);  // 2 条边

    DestroyMSTResult(&res_k);
    DestroyMSTResult(&res_p);
    algraph_destroy(g);
  }

  SECTION("Disconnected Forest: Two Separate Components") {
    // C1: 0-1 (10.0)
    // C2: 2-3 (5.0), 3-4 (2.0)
    // 预期: 2个分量, 3条边, 总重 17.0
    std::vector<Edge> edges = {{0, 1, 10.0}, {2, 3, 5.0}, {3, 4, 2.0}};
    ALGraph* g = BuildTestGraph(5, edges, algraph_init, algraph_destroy);
    printf("Running Kruskal\n");
    MSTResult* res_k = Kruskal(g, &g->bg);
    printf("Running Prim\n");
    MSTResult* res_p = Prim(g, &g->bg);

    CHECK(res_k->component_count == 2);
    CHECK(res_p->component_count == 2);
    CHECK(res_k->edge_count == 3);
    CHECK(GetTotalWeight(res_k) == Approx(17.0));

    // 验证第二个分量的边是否被正确重排 (通过 offset)
    int c2_start = res_k->component_offset[1];
    int c2_end = res_k->component_offset[2];
    CHECK((c2_end - c2_start) == 2);  // 第二个分量有2条边

    DestroyMSTResult(&res_k);
    DestroyMSTResult(&res_p);
    algraph_destroy(g);
  }
}

TEST_CASE("MST Cross-Model Consistency", "[mst]") {
  std::vector<Edge> edges = {{0, 1, 1.0}, {1, 2, 2.0}, {0, 2, 0.5}};

  SECTION("ALGraph Model") {
    // 使用方案 A 的调用方式
    auto* g = BuildTestGraph<ALGraph>(3, edges, algraph_init, algraph_destroy);
    printf("Graph built\n");
    REQUIRE(g != nullptr);
    printf("Running Kruskal\n");
    MSTResult* res = Kruskal(g, &g->bg);
    CHECK(res->edge_count == 2);

    algraph_destroy(g);
    DestroyMSTResult(&res);
    // 注意：别忘了释放 res 里的 edges 和 offset
  }
}
