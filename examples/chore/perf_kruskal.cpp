#include <chrono>
#include <iostream>
#include <random>
#include <set>
#include <vector>

#include "graph/ALGraph.h"
#include "graph/GraphAlgorithms.h"
#include "graph/MGraph.h"

template <typename GType>
void SetupGraph(GType* g, int nv, int ne, BaseGraph* bg) {
  // 1. 统一添加顶点
  for (int i = 0; i < nv; ++i) {
    bg->mops.add_vert(g);
  }

  long long max_edges = (long long)nv * (nv - 1) / 2;
  if (ne > max_edges) ne = (int)max_edges;
  if (ne <= 0) return;

  std::mt19937 rng(42);
  std::uniform_real_distribution<double> weight_dist(1.0, 10.0);

  // 2. 决策：如果密度超过 25%，使用全量洗牌法；否则使用随机采样法
  double density = (double)ne / max_edges;

  if (density > 0.25) {
    // --- 稠密图优化：洗牌法 ---
    std::vector<std::pair<int, int>> all_possible_edges;
    all_possible_edges.reserve(max_edges);
    for (int i = 0; i < nv; ++i) {
      for (int j = i + 1; j < nv; ++j) {
        all_possible_edges.emplace_back(i, j);
      }
    }
    std::shuffle(all_possible_edges.begin(), all_possible_edges.end(), rng);

    for (int i = 0; i < ne; ++i) {
      bg->mops.add_edge(g, all_possible_edges[i].first,
                        all_possible_edges[i].second, weight_dist(rng));
    }
  } else {
    // --- 稀疏图优化：Set 随机采样法 ---
    std::set<std::pair<int, int>> seen;
    std::uniform_int_distribution<int> dist(0, nv - 1);
    int edges_added = 0;

    while (edges_added < ne) {
      int u = dist(rng);
      int v = dist(rng);
      if (u == v) continue;
      if (u > v) std::swap(u, v);

      // 使用 emplace 尝试插入，减少 find + insert 的重复搜索
      auto [it, inserted] = seen.emplace(u, v);
      if (inserted) {
        bg->mops.add_edge(g, u, v, weight_dist(rng));
        edges_added++;
      }
    }
  }
}
double profile_kruskal(int nv, int ne, bool use_matrix) {
  // 1. 初始化与构图
  void* g;
  BaseGraph* bg;
  if (use_matrix) {
    MGraph* mg = mgraph_init(nv);
    mg->n_verts = nv;
    mg->directed = false;  // 确保初始化正确
    SetupGraph(mg, nv, ne, &mg->bg);
    g = mg;
    bg = &mg->bg;
  } else {
    ALGraph* alg = algraph_init(nv);
    alg->directed = false;
    SetupGraph(alg, nv, ne, &alg->bg);
    g = alg;
    bg = &alg->bg;
  }

  // 2. 计时 Kruskal
  auto start = std::chrono::high_resolution_clock::now();

  auto res = Kruskal(g, bg);

  auto end = std::chrono::high_resolution_clock::now();
  DestroyMSTResult(&res);

  // 3. 释放内存
  if (use_matrix)
    mgraph_destroy((MGraph*)g);
  else
    algraph_destroy((ALGraph*)g);

  std::chrono::duration<double, std::micro> diff = end - start;
  return diff.count();  // 返回微秒
}

int main(int argc, char** argv) {
  if (argc < 3) return 1;
  int nv = std::stoi(argv[1]);
  int ne = std::stoi(argv[2]);
  int repeats = 10;

  double total_al = 0, total_mg = 0;
  for (int i = 0; i < repeats; ++i) {
    total_al += profile_kruskal(nv, ne, false);
    total_mg += profile_kruskal(nv, ne, true);
  }

  // 输出格式: ne, al_time_avg, mg_time_avg
  std::cout << ne << "," << (total_al / repeats) << "," << (total_mg / repeats)
            << std::endl;
  return 0;
}
