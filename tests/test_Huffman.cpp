#include <catch2/catch.hpp>

#include "tree/Huffman.hpp"

// 辅助函数：方便在测试中获取 Node data 里的 int 值
int GetWeight(Node* node) {
  if (node == NULL) return -1;
  return *(int*)(node->data);
}

// 辅助函数：判断是否为叶子节点
bool IsLeaf(Node* node) {
  return node != NULL && node->l == NULL && node->r == NULL;
}

TEST_CASE("Huffman Tree Building", "[huffman]") {
  SECTION("Empty array should return NULL") {
    int w[] = {};
    BiTreeLi root = BuildHuffman(w, 0);
    REQUIRE(root == NULL);
  }

  SECTION("Single element array should return a single root node") {
    int w[] = {42};
    BiTreeLi root = BuildHuffman(w, 1);

    REQUIRE(root != NULL);
    REQUIRE(GetWeight(root) == 42);
    REQUIRE(IsLeaf(root));  // 应该是叶子节点，没有子树
  }

  SECTION("Normal array: properly combines weights and builds structure") {
    // 初始权重： 5, 4, 3, 2
    // 排序后应该是：2, 3, 4, 5
    // 合并过程：
    // 1. 最小两个是 2 和 3，合并成 5（内部节点）
    // 2. 剩下节点池: 4, 5(原), 5(新)
    // 3. 最小两个是 4 和 5(原)，合并成 9（内部节点）
    // 4. 剩下节点池: 5(新), 9(新)
    // 5. 合并成最终根节点 14
    int w[] = {5, 4, 3, 2};
    BiTreeLi root = BuildHuffman(w, 4);

    REQUIRE(root != NULL);
    REQUIRE(GetWeight(root) == 14);  // 5 + 4 + 3 + 2 = 14
    REQUIRE(root->l != NULL);
    REQUIRE(root->r != NULL);

    // 验证第一层子节点 (5 和 9)
    // 注意：具体哪个在左哪个在右取决于 _get_next_node 里的队列出队顺序
    // 按照现在的逻辑，ori 先出，所以 5(原) 会和 9 结合之前的 4 结合
    int w1 = GetWeight(root->l);
    int w2 = GetWeight(root->r);

    // 我们知道一边是 5，一边是 9
    bool has_5_and_9 = (w1 == 5 && w2 == 9) || (w1 == 9 && w2 == 5);
    REQUIRE(has_5_and_9);

    // 找到权重为 9 的那棵子树，它应该包含 4 和 5(原) 的叶子
    Node* node_9 = (w1 == 9) ? root->l : root->r;
    REQUIRE(GetWeight(node_9->l) + GetWeight(node_9->r) == 9);

    bool has_4_and_5 =
        (GetWeight(node_9->l) == 4 && GetWeight(node_9->r) == 5) ||
        (GetWeight(node_9->l) == 5 && GetWeight(node_9->r) == 4);
    REQUIRE(has_4_and_5);
    REQUIRE(IsLeaf(node_9->l));
    REQUIRE(IsLeaf(node_9->r));

    // 找到权重为 5 的那棵内部子树，它应该包含 2 和 3 的叶子
    Node* node_5 = (w1 == 5) ? root->l : root->r;
    REQUIRE(GetWeight(node_5->l) + GetWeight(node_5->r) == 5);

    bool has_2_and_3 =
        (GetWeight(node_5->l) == 2 && GetWeight(node_5->r) == 3) ||
        (GetWeight(node_5->l) == 3 && GetWeight(node_5->r) == 2);
    REQUIRE(has_2_and_3);
    REQUIRE(IsLeaf(node_5->l));
    REQUIRE(IsLeaf(node_5->r));
  }
}
