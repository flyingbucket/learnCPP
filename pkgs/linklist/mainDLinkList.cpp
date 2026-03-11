#include "DLinkList.hpp"
#include <cassert>
#include <cstdio>
#include <cstdlib>

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

  // 1. 测试初始化
  printf("--- Test 1: Initialization ---\n");
  if (InitDList(L)) {
    printf("Init Success.\n");
  }
  assert(L != NULL);
  assert(LengthWithoutHeadNode(L) == 0);
  PrintList(L);

  // 2. 测试插入
  printf("\n--- Test 2: Insertion ---\n");
  InsertPrior(L, 1, 10);
  InsertPrior(L, 2, 20);
  InsertPrior(L, 2, 15);

  PrintList(L);
  assert(LengthWithoutHeadNode(L) == 3);
  assert(GetElem(L, 1)->data == 10);
  assert(GetElem(L, 2)->data == 15);

  // 3. 测试查找
  printf("\n--- Test 3: Search ---\n");
  DLNode *target = LocateElem(L, 15);
  assert(target != NULL && target->data == 15);
  printf("LocateElem(15) found. Prior is %d\n", target->prior->data);

  // 4. 测试删除
  printf("\n--- Test 4: Deletion ---\n");
  bool del_ok = DeletePrior(L, 2, 0);
  assert(del_ok == true);
  printf("After DeletePrior(index=2) [Deleted 10]:\n");
  PrintList(L);
  assert(GetElem(L, 1)->data == 15);

  // 5. 测试边界
  printf("\n--- Test 5: Boundary Conditions ---\n");
  assert(DeletePrior(L, 1, 0) == false);
  printf("DeletePrior with index 1 correctly failed.\n");
  assert(InsertPrior(L, 0, 99) == false);
  printf("InsertPrior with index 0 correctly failed.\n");

  DestroyList(L);
  printf("\nTest Suite Passed!\n");

  return 0;
}
