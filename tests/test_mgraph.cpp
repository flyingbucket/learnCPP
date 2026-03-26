#include <catch2/catch.hpp>

#include "graph/GraphOps.h"
#include "graph/MGraph.h"

TEST_CASE("MGraph Operations", "[MGraph]") {
  int n_verts = 5;
  MGraph* g = mgraph_init(n_verts);
  REQUIRE(g != nullptr);
  REQUIRE(g->adj != nullptr);

  SECTION("Initialization and Info") {
    REQUIRE(g->bg.iops.vertex_count(g) == n_verts);
    REQUIRE(g->bg.iops.valid_vertex(g, 0) == true);
    REQUIRE(g->bg.iops.valid_vertex(g, 4) == true);
    REQUIRE(g->bg.iops.valid_vertex(g, 5) == false);
  }

  SECTION("Edge Operations: Add, Remove and Weight") {
    VertexId v1 = 1;
    VertexId v2 = 2;

    bool edge_added = g->bg.mops.add_edge(g, v1, v2, 3.5);
    // 如果这里失败，请检查 MGraph.c 中的 m_add_edge 边界判断
    REQUIRE(edge_added == true);

    REQUIRE(g->bg.qops.adjacent(g, v1, v2) == true);

    Weight w = g->bg.wops.get_edge_weight(g, v1, v2);
    REQUIRE(w == 3.5);

    g->bg.wops.update_edge_weight(g, v1, v2, 8.0);
    REQUIRE(g->bg.wops.get_edge_weight(g, v1, v2) == 8.0);

    bool edge_removed = g->bg.mops.remove_edge(g, v1, v2);
    REQUIRE(edge_removed == true);

    REQUIRE(g->bg.qops.adjacent(g, v1, v2) == false);
    REQUIRE(g->bg.wops.get_edge_weight(g, v1, v2) == 0.0);
  }

  SECTION("Query Operations: Neighbors") {
    // 先手动赋值绕过 m_add_edge 可能存在的 bug 进行后续测试
    g->adj[1 * n_verts + 2] = 1.0;
    g->adj[1 * n_verts + 4] = 1.0;

    Edge first_edge = g->bg.qops.first_neighbor(g, 1);
    REQUIRE(first_edge.h == 2);

    Edge next = g->bg.qops.next_neighbor(g, 1, 2);
    REQUIRE(next.h == 4);

    Edge none = g->bg.qops.next_neighbor(g, 1, 4);
    REQUIRE(none.h == -1);
  }

  mgraph_destroy(g);
}
