#include <catch2/catch.hpp>

#include "graph/GraphOps.h"

extern "C" {
#include "graph/ALGraph.h"
#include "graph/AMLGraph.h"
#include "graph/CLGraph.h"
#include "graph/MGraph.h"
}

// ---------------------------------------------------------
// 测试 1: ALGraph (邻接表) - 动态添加顶点与有向边
// ---------------------------------------------------------
TEST_CASE("ALGraph Container Semantics", "[algraph]") {
  ALGraph* raw_g = algraph_init(4);
  REQUIRE(raw_g != nullptr);

  // 模拟多态调用：仅通过 void* 和 BaseGraph* 接口操作
  void* g = raw_g;
  BaseGraph* ops = &raw_g->bg;

  SECTION("Init semantic: Graph should be empty") {
    REQUIRE(ops->iops.vertex_count(g) == 0);
  }

  SECTION("Dynamic building semantic: Add vertices and edges") {
    VertexId v0 = ops->mops.add_vert(g);
    VertexId v1 = ops->mops.add_vert(g);
    VertexId v2 = ops->mops.add_vert(g);

    REQUIRE(v0 == 0);
    REQUIRE(v2 == 2);
    REQUIRE(ops->iops.vertex_count(g) == 3);

    // ALGraph 默认实现为有向图添加边
    REQUIRE(ops->mops.add_edge(g, v0, v1, 1.5) == true);
    REQUIRE(ops->mops.add_edge(g, v0, v2, 2.5) == true);

    // 验证边和权重
    REQUIRE(ops->qops.adjacent(g, v0, v1) == true);
    REQUIRE(ops->qops.adjacent(g, v1, v0) == false);  // 有向的证明

    REQUIRE(ops->wops.get_edge_weight(g, v0, v1) == Catch::Detail::Approx(1.5));

    // 验证迭代邻居
    Edge first_edge = ops->qops.first_neighbor(g, v0);
    VertexId first = first_edge.h;
    REQUIRE((first == v1 ||
             first == v2));  // 取决于头插法，实际上应该是最后插入的 v2
  }

  algraph_destroy(raw_g);
}

// ---------------------------------------------------------
// 测试 2: AMLGraph (邻接多重表) - 无向图语义
// ---------------------------------------------------------
TEST_CASE("AMLGraph Container Semantics", "[amlgraph]") {
  AMLGraph* raw_g = amlgraph_init(4);
  REQUIRE(raw_g != nullptr);

  void* g = raw_g;
  BaseGraph* ops = &raw_g->bg;

  SECTION("Init semantic: Graph should be empty") {
    REQUIRE(ops->iops.vertex_count(g) == 0);
  }

  SECTION("Undirected Edge Building") {
    VertexId v0 = ops->mops.add_vert(g);
    VertexId v1 = ops->mops.add_vert(g);

    // 添加无向边
    REQUIRE(ops->mops.add_edge(g, v0, v1, 5.0) == true);

    // 验证无向性：正反向都应该 adjacent
    REQUIRE(ops->qops.adjacent(g, v0, v1) == true);
    REQUIRE(ops->qops.adjacent(g, v1, v0) == true);

    // 验证权重更新
    REQUIRE(ops->wops.update_edge_weight(g, v0, v1, 10.0) ==
            Catch::Detail::Approx(5.0));
    REQUIRE(ops->wops.get_edge_weight(g, v1, v0) ==
            Catch::Detail::Approx(10.0));
  }

  amlgraph_destroy(raw_g);
}

// ---------------------------------------------------------
// 测试 3: CLGraph (十字链表) - 强大的有向图操作
// ---------------------------------------------------------
TEST_CASE("CLGraph Container Semantics", "[clgraph]") {
  CLGraph* raw_g = clgraph_init(4);
  REQUIRE(raw_g != nullptr);

  void* g = raw_g;
  BaseGraph* ops = &raw_g->bg;

  SECTION("Dynamic building and edge removal") {
    VertexId v0 = ops->mops.add_vert(g);
    VertexId v1 = ops->mops.add_vert(g);

    REQUIRE(ops->mops.add_edge(g, v0, v1, 3.0) == true);
    REQUIRE(ops->qops.adjacent(g, v0, v1) == true);

    // 删除边
    REQUIRE(ops->mops.remove_edge(g, v0, v1) == true);
    REQUIRE(ops->qops.adjacent(g, v0, v1) == false);
    REQUIRE(ops->wops.get_edge_weight(g, v0, v1) == -1);  // 假设没找到返回-1
  }

  destroy_clgraph(raw_g);
}

// ---------------------------------------------------------
// 测试 4: MGraph (邻接矩阵) - 特殊的静态容器语义
// ---------------------------------------------------------
TEST_CASE("MGraph Container Semantics", "[mgraph]") {
  // MGraph 初始化直接指定容量并分配，它不支持动态扩容顶点
  MGraph* raw_g = mgraph_init(3);
  REQUIRE(raw_g != nullptr);

  void* g = raw_g;
  BaseGraph* ops = &raw_g->bg;

  SECTION("Static container semantics") {
    // 一初始化，点就固定了
    REQUIRE(ops->iops.vertex_count(g) == 3);

    // 尝试添加顶点应该返回 -1 (根据你打印警告并返回-1的实现)
    REQUIRE(ops->mops.add_vert(g) == -1);
  }

  SECTION("Edge manipulation") {
    // 直接在已有顶点上加边
    REQUIRE(ops->mops.add_edge(g, 0, 1, 9.9) == true);
    REQUIRE(ops->qops.adjacent(g, 0, 1) == true);
    REQUIRE(ops->wops.get_edge_weight(g, 0, 1) == Catch::Detail::Approx(9.9));

    // 无效顶点添加边应该失败
    REQUIRE(ops->mops.add_edge(g, 0, 99, 1.0) == false);
  }

  mgraph_destroy(raw_g);
}
