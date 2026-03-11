#include "DLinkList.hpp"
#include <cassert>
#include <cstdio>
#include <cstdlib>

#include <iostream>

// 一个简单的断言增强，方便定位问题
#define TEST_ASSERT(cond, msg)                                                 \
  if (!(cond)) {                                                               \
    std::cerr << "FAILED: " << msg << std::endl;                               \
    abort();                                                                   \
  } else {                                                                     \
    std::cout << "PASSED: " << msg << std::endl;                               \
  }
// 辅助函数：打印链表内容
void PrintList(DLinkList L) {
  DLNode *p = L->next;
  printf("List (len=%d): [Head] <-> ", L->data);
  while (p != NULL) {
    printf("%d <-> ", p->data);
    p = p->next;
  }
  printf("NULL\n");
}

// 辅助函数：清理内存
void DestroyList(DLinkList &L) {
  DLNode *p = L;
  while (p != NULL) {
    DLNode *temp = p;
    p = p->next;
    free(temp);
  }
  L = NULL;
}

int main() {
  DLinkList L;

  // 1. 初始化阶段
  InitDList(L);
  TEST_ASSERT(L != nullptr && L->next == nullptr, "Initialization");

  // 2. 插入阶段 (测试不同位置)
  InsertPrior(L, 1, 10); // [10]
  InsertPrior(L, 2, 20); // [10, 20]
  InsertPrior(L, 2, 15); // [10, 15, 20]
  TEST_ASSERT(LengthWithoutHeadNode(L) == 3,
              "Length after multiple insertions");

  // 3. 删除阶段 (重点测试引用安全)
  // 利用 C++17 作用域语法，确保 deleted_val 不会泄露到后续逻辑
  printf("Before deletion");
  PrintList(L);
  if (int deleted_val; DeletePrior(L, 2, deleted_val)) {
    TEST_ASSERT(deleted_val == 10, "Value captured from deletion");
    TEST_ASSERT(LengthWithoutHeadNode(L) == 2, "Length after deletion");
  } else {
    TEST_ASSERT(false, "Deletion should have succeeded");
  }

  // 4. 边界与防御性测试
  // 测试：删除不存在的位置
  int dummy;
  TEST_ASSERT(DeletePrior(L, 99, dummy) == false, "Delete out of range");

  // 测试：删除头节点的前驱（通常是非法操作）
  TEST_ASSERT(DeletePrior(L, 1, dummy) == false, "Delete head node's prior");

  // 5. 内存清理验证
  DestroyList(L);
  TEST_ASSERT(L == nullptr, "List destruction");

  std::cout << "\n==============================\n";
  std::cout << "All test cases passed successfully!" << std::endl;
  std::cout << "==============================\n";

  return 0;
}
