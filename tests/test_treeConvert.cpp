#include <catch2/catch.hpp>

#include "tree/convert.hpp"  // 包含你的头文件

// --- 辅助函数：创建一般树节点 ---
GeneralTree* create_gen_node(void* data) {
  GeneralTree* node = (GeneralTree*)malloc(sizeof(GeneralTree));
  node->data = data;
  node->childs = NULL;
  node->childs_count = 0;
  node->childs_capacity = 0;
  return node;
}

// --- 辅助函数：为一般树添加子节点 ---
void add_child(GeneralTree* parent, GeneralTree* child) {
  if (parent->childs_count == parent->childs_capacity) {
    parent->childs_capacity =
        parent->childs_capacity == 0 ? 4 : parent->childs_capacity * 2;
    parent->childs = (GeneralTree**)realloc(
        parent->childs, parent->childs_capacity * sizeof(GeneralTree*));
  }
  parent->childs[parent->childs_count++] = child;
}

// --- 辅助函数：释放内存，防止测试内存泄漏 ---
void free_gen_tree(GeneralTree* root) {
  if (!root) return;
  for (int i = 0; i < root->childs_count; ++i) {
    free_gen_tree(root->childs[i]);
  }
  free(root->childs);
  free(root);
}

void free_bi_tree(Node* root) {
  if (!root) return;
  free_bi_tree(root->l);
  free_bi_tree(root->r);
  free(root);
}

// ---------------- 测试用例 ----------------

TEST_CASE("GeneralTree to BinaryTree Conversion", "[tree][convert]") {
  // 准备测试数据
  int d[] = {0, 1, 2, 3, 4, 5, 6};

  SECTION("空树转换") {
    BiTreeLi bi_root = GeneralTree2BiTree(NULL);
    REQUIRE(bi_root == NULL);
  }

  SECTION("只有根节点的树") {
    GeneralTree* gen_root = create_gen_node(&d[1]);

    BiTreeLi bi_root = GeneralTree2BiTree(gen_root);

    REQUIRE(bi_root != NULL);
    REQUIRE(*(int*)(bi_root->data) == 1);
    REQUIRE(bi_root->l == NULL);
    REQUIRE(bi_root->r == NULL);

    free_gen_tree(gen_root);
    free_bi_tree(bi_root);
  }

  SECTION("扁平树：一个根节点，三个子节点") {
    // 结构:
    //    1
    //  / | \
    // 2  3  4
    GeneralTree* gen_root = create_gen_node(&d[1]);
    add_child(gen_root, create_gen_node(&d[2]));
    add_child(gen_root, create_gen_node(&d[3]));
    add_child(gen_root, create_gen_node(&d[4]));

    BiTreeLi bi_root = GeneralTree2BiTree(gen_root);

    // 期望的二叉树结构 (左孩子右兄弟):
    //    1
    //   /
    //  2
    //   \
    //    3
    //     \
    //      4
    REQUIRE(bi_root != NULL);
    REQUIRE(*(int*)(bi_root->data) == 1);

    Node* n2 = bi_root->l;
    REQUIRE(n2 != NULL);
    REQUIRE(*(int*)(n2->data) == 2);
    REQUIRE(n2->l == NULL);  // 2没有孩子

    Node* n3 = n2->r;
    REQUIRE(n3 != NULL);
    REQUIRE(*(int*)(n3->data) == 3);
    REQUIRE(n3->l == NULL);  // 3没有孩子

    Node* n4 = n3->r;
    REQUIRE(n4 != NULL);
    REQUIRE(*(int*)(n4->data) == 4);
    REQUIRE(n4->l == NULL);  // 4没有孩子
    REQUIRE(n4->r == NULL);  // 4没有下一个兄弟

    free_gen_tree(gen_root);
    free_bi_tree(bi_root);
  }

  SECTION("复杂多层树转换") {
    // 结构:
    //       1
    //     / | \
    //    2  3  4
    //      / \
    //     5   6
    GeneralTree* gen_root = create_gen_node(&d[1]);

    GeneralTree* n2 = create_gen_node(&d[2]);
    GeneralTree* n3 = create_gen_node(&d[3]);
    GeneralTree* n4 = create_gen_node(&d[4]);
    GeneralTree* n5 = create_gen_node(&d[5]);
    GeneralTree* n6 = create_gen_node(&d[6]);

    add_child(gen_root, n2);
    add_child(gen_root, n3);
    add_child(gen_root, n4);
    add_child(n3, n5);
    add_child(n3, n6);

    BiTreeLi bi_root = GeneralTree2BiTree(gen_root);

    // 期望的二叉树结构:
    //       1
    //      /
    //     2
    //      \
    //       3
    //      / \
    //     5   4
    //      \
    //       6
    REQUIRE(*(int*)(bi_root->data) == 1);

    Node* b2 = bi_root->l;
    REQUIRE(*(int*)(b2->data) == 2);

    Node* b3 = b2->r;
    REQUIRE(*(int*)(b3->data) == 3);

    Node* b4 = b3->r;
    REQUIRE(*(int*)(b4->data) == 4);
    REQUIRE(b4->r == NULL);

    Node* b5 = b3->l;  // 3的大儿子是5
    REQUIRE(*(int*)(b5->data) == 5);

    Node* b6 = b5->r;  // 5的兄弟是6
    REQUIRE(*(int*)(b6->data) == 6);
    REQUIRE(b6->r == NULL);
    REQUIRE(b6->l == NULL);

    free_gen_tree(gen_root);
    free_bi_tree(bi_root);
  }
}
