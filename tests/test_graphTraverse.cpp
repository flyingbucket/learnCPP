#include <iostream>
#include <vector>

#include "catch2/catch.hpp"

// 包含你的头文件
extern "C" {
#include "graph/GraphAlgorithms.h"
#include "graph/GraphOps.h"
}

// ============================================================================
// 1. 构建 Mock Graph (模拟图) 用于测试
// ============================================================================
#define MAX_MOCK_VERTS 20

const Edge invalid_edge = {.t = -1, .h = -1, .w = INFINITY};
struct MockGraph {
  BaseGraph bg;
  int n_verts;
  int adj[MAX_MOCK_VERTS][MAX_MOCK_VERTS];  // 简易邻接矩阵
};

// --- Mock GraphOps 实现 ---
static int mock_vertex_count(void* G) {
  return static_cast<MockGraph*>(G)->n_verts;
}

static bool mock_valid_vertex(void* G, VertexId v) {
  return v >= 0 && v < static_cast<MockGraph*>(G)->n_verts;
}

static Edge mock_first_neighbor(void* G, VertexId v) {
  MockGraph* mg = static_cast<MockGraph*>(G);
  for (int i = 0; i < mg->n_verts; i++) {
    if (mg->adj[v][i]) {
      Weight w = mg->adj[v][i];
      Edge res = {
          .t = v,
          .h = i,
          .w = w,
      };
      return res;
    }
  }
  return invalid_edge;
}

static Edge mock_next_neighbor(void* G, VertexId v, VertexId w) {
  MockGraph* mg = static_cast<MockGraph*>(G);
  for (int i = w + 1; i < mg->n_verts; i++) {
    if (mg->adj[v][i]) {
      Weight weight = mg->adj[v][i];
      Edge res = {
          .t = v,
          .h = i,
          .w = weight,
      };
      return res;
    }
  }
  return invalid_edge;
}

static Weight mock_get_edge_weight(void* G, VertexId v1, VertexId v2) {
  return 1.0;  // 默认权重为 1
}

// --- Mock Graph 辅助工厂函数 ---
MockGraph create_mock_graph(int n) {
  MockGraph mg;
  mg.n_verts = n;
  for (int i = 0; i < MAX_MOCK_VERTS; ++i)
    for (int j = 0; j < MAX_MOCK_VERTS; ++j) mg.adj[i][j] = 0;

  // 绑定函数指针
  mg.bg.iops.vertex_count = mock_vertex_count;
  mg.bg.iops.valid_vertex = mock_valid_vertex;
  mg.bg.qops.first_neighbor = mock_first_neighbor;
  mg.bg.qops.next_neighbor = mock_next_neighbor;
  mg.bg.wops.get_edge_weight = mock_get_edge_weight;
  return mg;
}

void add_edge(MockGraph& mg, int u, int v) {
  mg.adj[u][v] = 1;
  mg.adj[v][u] = 1;  // 假定为无向图
}

// ============================================================================
// 2. 定义测试用的 Visitor 回调
// ============================================================================

// 记录访问顺序的回调函数
static void record_vertex_visit(void* G, const BaseGraph* bg, VertexId v,
                                void* ctx) {
  auto* order = static_cast<std::vector<VertexId>*>(ctx);
  order->push_back(v);
}

// ============================================================================
// 3. Catch2 测试用例
// ============================================================================

TEST_CASE("BFS Traversal Order", "[BFS]") {
  MockGraph mg = create_mock_graph(5);
  /* 构建测试图结构:
        0
       / \
      1   2
     /     \
    3       4
  */
  add_edge(mg, 0, 1);
  add_edge(mg, 0, 2);
  add_edge(mg, 1, 3);
  add_edge(mg, 2, 4);

  std::vector<VertexId> visit_order;
  VisitorClosure visitor = {nullptr, record_vertex_visit, &visit_order};
  bool visited[MAX_MOCK_VERTS] = {false};

  SECTION("Starting from vertex 0") {
    BFS(&mg, &mg.bg, 0, visited, visitor);

    REQUIRE(visit_order.size() == 5);
    REQUIRE(visit_order[0] == 0);  // 起点

    // 由于邻接矩阵顺序，0 的邻居 1 和 2 会先入队
    // (顺序取决于矩阵遍历顺序，这里一定是 1 然后 2)
    REQUIRE(visit_order[1] == 1);
    REQUIRE(visit_order[2] == 2);

    // 然后是 1 的邻居 3，和 2 的邻居 4
    REQUIRE(visit_order[3] == 3);
    REQUIRE(visit_order[4] == 4);
  }
}

TEST_CASE("DFS Traversal Order", "[DFS]") {
  MockGraph mg = create_mock_graph(5);
  /* 构建测试图结构:
        0
       / \
      1---2
       \
        3---4
  */
  add_edge(mg, 0, 1);
  add_edge(mg, 0, 2);
  add_edge(mg, 1, 2);
  add_edge(mg, 1, 3);
  add_edge(mg, 3, 4);

  std::vector<VertexId> visit_order;
  VisitorClosure visitor = {nullptr, record_vertex_visit, &visit_order};
  bool visited[MAX_MOCK_VERTS] = {false};

  SECTION("Starting from vertex 0") {
    DFS(&mg, &mg.bg, 0, visited, visitor);

    REQUIRE(visit_order.size() == 5);

    // 期望的深度优先顺序: 0 -> 1 -> 2 -> 3 -> 4
    // 解析:
    // - 0 首个邻居是 1
    // - 1 首个邻居是 0 (已访问), 下一个是 2
    // - 2 邻居是 0, 1 (均已访问), 触发回溯到 1
    // - 1 继续找邻居，找到 3
    // - 3 邻居找到 4
    std::vector<VertexId> expected = {0, 1, 2, 3, 4};
    REQUIRE(visit_order == expected);
  }
}

TEST_CASE("BFS Calculate Shortest Paths", "[BFS][ShortestPath]") {
  MockGraph mg = create_mock_graph(6);
  /* 构建具有多条路径的图:
        1 -- 2
       /      \
      0 -- 3 -- 4 -- 5
      (0 到 4 可以走 0->1->2->4(长3) 或者 0->3->4(长2))
  */
  add_edge(mg, 0, 1);
  add_edge(mg, 1, 2);
  add_edge(mg, 2, 4);

  add_edge(mg, 0, 3);
  add_edge(mg, 3, 4);
  add_edge(mg, 4, 5);

  int dists[MAX_MOCK_VERTS];

  // 初始化外层脏数据以验证算法覆盖
  for (int i = 0; i < 6; i++) dists[i] = -99;

  SECTION("Shortest path from 0") {
    BFSCalculateShortestPaths(&mg, &mg.bg, 0, dists);

    // 验证各点的最短跳数
    REQUIRE(dists[0] == 0);  // 起点
    REQUIRE(dists[1] == 1);
    REQUIRE(dists[3] == 1);

    REQUIRE(dists[2] == 2);  // 0->1->2
    REQUIRE(dists[4] == 2);  // 最短路径是 0->3->4，跳数为2
    REQUIRE(dists[5] == 3);  // 0->3->4->5
  }

  SECTION("Shortest path from disconnected node") {
    MockGraph mg_unconn = create_mock_graph(3);
    add_edge(mg_unconn, 0, 1);
    // 节点 2 是孤立点

    BFSCalculateShortestPaths(&mg_unconn, &mg_unconn.bg, 0, dists);

    REQUIRE(dists[0] == 0);
    REQUIRE(dists[1] == 1);
    REQUIRE(dists[2] == -1);  // 孤立点由于未被更新，应该保持初始化状态 -1
  }
}
