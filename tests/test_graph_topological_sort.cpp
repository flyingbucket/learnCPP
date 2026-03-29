#include <catch2/catch.hpp>
#include <map>
#include <vector>

extern "C" {
#include "graph/ALGraph.h"
#include "graph/GraphAlgorithms.h"
#include "graph/GraphOps.h"
#include "graph/MGraph.h"
}

// 辅助函数：验证拓扑排序的合法性
// 逻辑：遍历图中所有的边 (u, v)，确保在 sorted 数组中 u 的下标 < v 的下标
bool IsValidTopologicalOrder(const std::vector<VertexId>& res, const void* g,
                             const BaseGraph* bg) {
  int nv = bg->iops.vertex_count(g);
  if (res.size() != (size_t)nv) return false;

  // 建立一个 值 -> 索引 的映射表，方便快速查找位置
  std::map<VertexId, int> pos;
  for (int i = 0; i < nv; ++i) {
    pos[res[i]] = i;
  }

  // 遍历矩阵中的每一条边
  Weight* mat = bg->qops.get_materix(g);
  bool valid = true;
  for (int i = 0; i < nv; ++i) {
    for (int j = 0; j < nv; ++j) {
      // 如果存在边 i -> j
      if (mat[i * nv + j] > 0) {
        // 验证 i 是否在 j 之前
        if (pos[i] >= pos[j]) {
          valid = false;
          break;
        }
      }
    }
  }
  free(mat);
  return valid;
}

TEST_CASE("Topological Sort with ALGraph", "[toposort][algraph]") {
  SECTION("DAG - Should return a valid topological order") {
    ALGraph* g = algraph_init(4, true);
    for (int i = 0; i < 4; i++) g->bg.mops.add_vert(g);

    /* 构造图:
       0 ---> 1 ---> 3
        \---> 2 ---/
    */
    g->bg.mops.add_edge(g, 0, 1, 1.0);
    g->bg.mops.add_edge(g, 0, 2, 1.0);
    g->bg.mops.add_edge(g, 1, 3, 1.0);
    g->bg.mops.add_edge(g, 2, 3, 1.0);

    VertexId* sorted = TopologicalSort(g, &(g->bg));

    REQUIRE(sorted != nullptr);

    std::vector<VertexId> res(sorted, sorted + 4);

    // 使用通用逻辑验证：所有边方向必须符合索引顺序
    REQUIRE(IsValidTopologicalOrder(res, g, &(g->bg)));

    free(sorted);
    algraph_destroy(g);
  }

  SECTION("Graph with Cycle - Should detect cycle and return NULL") {
    ALGraph* g = algraph_init(3, true);
    for (int i = 0; i < 3; i++) g->bg.mops.add_vert(g);

    g->bg.mops.add_edge(g, 0, 1, 1.0);
    g->bg.mops.add_edge(g, 1, 2, 1.0);
    g->bg.mops.add_edge(g, 2, 0, 1.0);

    VertexId* sorted = TopologicalSort(g, &(g->bg));
    REQUIRE(sorted == nullptr);

    algraph_destroy(g);
  }
}

TEST_CASE("Topological Sort with MGraph", "[toposort][mgraph]") {
  SECTION("DAG - Comprehensive path check") {
    int nv = 6;
    MGraph* g = mgraph_init(nv);
    g->directed = true;

    // 构造一个稍微复杂的依赖关系
    // 5 -> 2 -> 3 -> 1
    // 5 -> 0 -> 4 -> 1
    g->bg.mops.add_edge(g, 5, 2, 1.0);
    g->bg.mops.add_edge(g, 5, 0, 1.0);
    g->bg.mops.add_edge(g, 4, 1, 1.0);
    g->bg.mops.add_edge(g, 2, 3, 1.0);
    g->bg.mops.add_edge(g, 3, 1, 1.0);
    g->bg.mops.add_edge(g, 0, 4, 1.0);

    VertexId* sorted = TopologicalSort(g, &(g->bg));
    REQUIRE(sorted != nullptr);

    std::vector<VertexId> res(sorted, sorted + nv);

    // 验证整体拓扑序合法性
    REQUIRE(IsValidTopologicalOrder(res, g, &(g->bg)));

    free(sorted);
    mgraph_destroy(g);
  }
}
