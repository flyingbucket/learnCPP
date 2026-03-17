#include <vector>

#include "catch2/catch.hpp"
#include "tree/BinaryTree.hpp"

// ==========================================
// 辅助工具：用于捕获遍历结果
// ==========================================

// 全局变量用于存储遍历结果，每次测试前需要清空
std::vector<int> g_result;

// 1. 针对链表树的捕获访问者
void capture_li_visit(void* tree, void* node_handle) {
  Node* node = (Node*)node_handle;
  if (node && node->data) {
    g_result.push_back(*(int*)(node->data));
  }
}

// 2. 针对数组树的捕获访问者
void capture_arr_visit(void* tree, void* node_handle) {
  BiTreeArr* arrTree = (BiTreeArr*)tree;
  intptr_t index = (intptr_t)node_handle;

  // 计算内存地址并读取 int
  char* target = (char*)arrTree->arr + (index * arrTree->elem_size);
  g_result.push_back(*(int*)target);
}

// ==========================================
// 测试用例
// ==========================================

TEST_CASE("Linked List Binary Tree (Node*)", "[list]") {
  // 准备数据
  // 结构:      1
  //          /   \
  //         2     3
  //        /
  //       4

  int d1 = 1, d2 = 2, d3 = 3, d4 = 4;

  Node n4 = {&d4, NULL, NULL};
  Node n2 = {&d2, &n4, NULL};  // 2 的左孩子是 4
  Node n3 = {&d3, NULL, NULL};
  Node root = {&d1, &n2, &n3};  // 1 的左是 2，右是 3

  // 每次测试前清空结果
  g_result.clear();

  SECTION("PreOrder Traversal") {
    // 预期: 1 -> 2 -> 4 -> 3
    PreOrder(NULL, &root, BiTreeLiOps, capture_li_visit);

    REQUIRE(g_result.size() == 4);
    REQUIRE(g_result == std::vector<int>{1, 2, 4, 3});
  }

  SECTION("InOrder Traversal") {
    // 预期: 4 -> 2 -> 1 -> 3
    InOrder(NULL, &root, BiTreeLiOps, capture_li_visit);

    REQUIRE(g_result.size() == 4);
    REQUIRE(g_result == std::vector<int>{4, 2, 1, 3});
  }

  SECTION("PostOrder Traversal") {
    // 预期: 4 -> 2 -> 3 -> 1
    PostOrder(NULL, &root, BiTreeLiOps, capture_li_visit);

    REQUIRE(g_result.size() == 4);
    REQUIRE(g_result == std::vector<int>{4, 2, 3, 1});
  }
}

TEST_CASE("Array Binary Tree (BiTreeArr)", "[array]") {
  // 准备数据
  // 数组: [1, 2, 3, 4, 5]
  // 下标:  0, 1, 2, 3, 4
  //
  // 结构:       1 (idx 0)
  //           /   \
  //          2     3
  //         / \
  //        4   5

  int data[] = {1, 2, 3, 4, 5};
  BiTreeArr arrTree;
  arrTree.arr = data;
  arrTree.elem_size = sizeof(int);
  arrTree.size = 5;  // 只有5个元素
  arrTree.capacity = 10;

  g_result.clear();

  // 数组树的根节点句柄通常是下标 0 (强转为 void*)
  void* root_handle = (void*)0;

  SECTION("PreOrder Traversal") {
    // 预期: 1 -> 2 -> 4 -> 5 -> 3
    PreOrder(&arrTree, root_handle, BiTreeArrOps, capture_arr_visit);

    REQUIRE(g_result.size() == 5);
    REQUIRE(g_result == std::vector<int>{1, 2, 4, 5, 3});
  }

  SECTION("InOrder Traversal") {
    // 预期: 4 -> 2 -> 5 -> 1 -> 3
    InOrder(&arrTree, root_handle, BiTreeArrOps, capture_arr_visit);

    REQUIRE(g_result.size() == 5);
    REQUIRE(g_result == std::vector<int>{4, 2, 5, 1, 3});
  }

  SECTION("PostOrder Traversal") {
    // 预期: 4 -> 5 -> 2 -> 3 -> 1
    PostOrder(&arrTree, root_handle, BiTreeArrOps, capture_arr_visit);

    REQUIRE(g_result.size() == 5);
    REQUIRE(g_result == std::vector<int>{4, 5, 2, 3, 1});
  }
}

TEST_CASE("Edge Cases", "[edge]") {
  g_result.clear();

  SECTION("Empty Linked List Tree") {
    PreOrder(NULL, NULL, BiTreeLiOps, capture_li_visit);
    REQUIRE(g_result.empty());
  }

  SECTION("Empty Array Tree") {
    BiTreeArr arrTree = {NULL, sizeof(int), 10, 0};  // size = 0
    PreOrder(&arrTree, (void*)0, BiTreeArrOps, capture_arr_visit);
    REQUIRE(g_result.empty());
  }

  SECTION("Array Tree with Missing Children (Sparse)") {
    // 数组: [1, 2, 3] 但是 size 只设为 2
    // 这意味着 3 (下标2) 应该被忽略，因为它 >= size
    // 结构:   1
    //        /
    //       2   (3被截断)

    int data[] = {1, 2, 3};
    BiTreeArr arrTree;
    arrTree.arr = data;
    arrTree.elem_size = sizeof(int);
    arrTree.size = 2;  // 只包含前两个元素

    InOrder(&arrTree, (void*)0, BiTreeArrOps, capture_arr_visit);

    REQUIRE(g_result.size() == 2);
    REQUIRE(g_result == std::vector<int>{2, 1});
  }
}
