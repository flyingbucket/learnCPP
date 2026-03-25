#include <algorithm>
#include <vector>

#include "catch2/catch.hpp"

extern "C" {
#include "graph/ALGraph.h"
#include "graph/GraphAlgorithms.h"
#include "graph/GraphOps.h"
}

// ============================================================================
// 测试辅助函数
// ============================================================================

// 辅助回调：将访问到的节点 ID 记录到 C++ 的 std::vector 中
static void record_vertex_visit(void* G, const BaseGraph* bg, VertexId v,
                                void* ctx) {
  auto* order = static_cast<std::vector<VertexId>*>(ctx);
  order->push_back(v);
}

// 辅助函数：手动添加节点并构建无向边
static void build_undirected_edge(ALGraph* alg, VertexId u, VertexId v,
                                  Weight w = 1.0) {
  BaseGraph* bg = &alg->bg;
  bg->mops.add_edge(alg, u, v, w);
  bg->mops.add_edge(alg, v, u, w);
}

// ============================================================================
// 测试用例 1: BFS 遍历机制测试
// ============================================================================
TEST_CASE("BFS Traversal Order with ALGraph (Strict Creation)",
          "[BFS][ALGraph]") {
  // 1. 分配空间 (此时 n_verts 为 0)
  ALGraph* alg = algraph_create(5, 10);
  BaseGraph* bg = &alg->bg;

  // 2. 严格遵循原则：手动添加节点，使 ID 与索引匹配
  for (int i = 0; i < 5; i++) {
    VertexId id = bg->mops.add_vert(alg);
    REQUIRE(id == i);  // 验证 add_vert 逻辑是否正确
  }

  /* 构建图结构:
        0
       / \
      1   2
     /     \
    3       4
  */
  build_undirected_edge(alg, 0, 1);
  build_undirected_edge(alg, 0, 2);
  build_undirected_edge(alg, 1, 3);
  build_undirected_edge(alg, 2, 4);

  std::vector<VertexId> visit_order;
  VisitorClosure visitor = {nullptr, record_vertex_visit, &visit_order};

  // 3. 确保 visited 数组干净
  std::vector<bool> visited(5, false);

  SECTION("Starting BFS from vertex 0") {
    // 注意：visited.data() 获取 bool 数组指针
    bool* v_ptr = new bool[5]{false};
    BFS(alg, bg, 0, v_ptr, visitor);

    REQUIRE(visit_order.size() == 5);
    REQUIRE(visit_order[0] == 0);

    // 验证层级关系
    std::vector<VertexId> level_1 = {visit_order[1], visit_order[2]};
    REQUIRE_THAT(level_1,
                 Catch::Matchers::UnorderedEquals(std::vector<VertexId>{1, 2}));

    std::vector<VertexId> level_2 = {visit_order[3], visit_order[4]};
    REQUIRE_THAT(level_2,
                 Catch::Matchers::UnorderedEquals(std::vector<VertexId>{3, 4}));

    delete[] v_ptr;
  }

  algraph_destroy(alg);
}

// ============================================================================
// 测试用例 2: DFS 遍历机制测试
// ============================================================================
TEST_CASE("DFS Traversal Order with ALGraph (Strict Creation)",
          "[DFS][ALGraph]") {
  ALGraph* alg = algraph_create(5, 10);
  BaseGraph* bg = &alg->bg;

  for (int i = 0; i < 5; i++) bg->mops.add_vert(alg);

  /* 构建图结构: 0 - 1 - 2, 1 - 3 - 4 */
  build_undirected_edge(alg, 0, 1);
  build_undirected_edge(alg, 1, 2);
  build_undirected_edge(alg, 1, 3);
  build_undirected_edge(alg, 3, 4);

  std::vector<VertexId> visit_order;
  VisitorClosure visitor = {nullptr, record_vertex_visit, &visit_order};

  SECTION("Starting DFS from vertex 0") {
    bool* v_ptr = new bool[5]{false};
    DFS(alg, bg, 0, v_ptr, visitor);

    REQUIRE(visit_order.size() == 5);
    REQUIRE(visit_order[0] == 0);

    std::vector<VertexId> expected_nodes = {0, 1, 2, 3, 4};
    REQUIRE_THAT(visit_order, Catch::Matchers::UnorderedEquals(expected_nodes));
    delete[] v_ptr;
  }

  algraph_destroy(alg);
}

// ============================================================================
// 测试用例 3: 基于 BFS 的单源最短路径 (SSSP)
// ============================================================================
TEST_CASE("BFS Shortest Paths with ALGraph (Strict Creation)",
          "[SSSP][ALGraph]") {
  ALGraph* alg = algraph_create(6, 20);
  BaseGraph* bg = &alg->bg;

  for (int i = 0; i < 6; i++) bg->mops.add_vert(alg);

  /* 0 -- 1 -- 2 -- 4
     |         |
     0 ------- 3 -- 4 -- 5
  */
  build_undirected_edge(alg, 0, 1);
  build_undirected_edge(alg, 1, 2);
  build_undirected_edge(alg, 2, 4);
  build_undirected_edge(alg, 0, 3);
  build_undirected_edge(alg, 3, 4);
  build_undirected_edge(alg, 4, 5);

  int dists[6];
  // BFSCalculateShortestPaths 内部会处理 dists 的初始化，但外部需保证空间足够
  BFSCalculateShortestPaths(alg, bg, 0, dists);

  SECTION("Validate distances") {
    REQUIRE(dists[0] == 0);
    REQUIRE(dists[1] == 1);
    REQUIRE(dists[3] == 1);
    REQUIRE(dists[2] == 2);
    REQUIRE(dists[4] == 2);  // 路径 0-3-4
    REQUIRE(dists[5] == 3);  // 路径 0-3-4-5
  }

  SECTION("Test disconnected component") {
    ALGraph* alg_unconn = algraph_create(3, 2);
    BaseGraph* bg_uc = &alg_unconn->bg;

    for (int i = 0; i < 3; i++) bg_uc->mops.add_vert(alg_unconn);
    bg_uc->mops.add_edge(alg_unconn, 0, 1, 1.0);

    int dists_unconn[3];
    BFSCalculateShortestPaths(alg_unconn, bg_uc, 0, dists_unconn);

    REQUIRE(dists_unconn[0] == 0);
    REQUIRE(dists_unconn[1] == 1);
    REQUIRE(dists_unconn[2] == -1);  // 孤立点

    algraph_destroy(alg_unconn);
  }

  algraph_destroy(alg);
}
