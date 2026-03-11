#include "CDLinkList.hpp" // 包含你的头文件
#include <cassert>
#include <cstdio> // 替换了 iostream

// 辅助函数：打印链表，方便肉眼调试
void PrintList(CDLinkList L) {
  if (L == NULL)
    return;
  printf("[Head(len=%d)] <-> ", L->data);

  DLNode *p = L->next;
  while (p != L) {
    printf("%d <-> ", p->data);
    p = p->next;
  }
  printf("(Back to Head)\n");
}

void TestInitAndLength() {
  printf("--- 运行测试: 初始化与长度 ---\n");
  CDLinkList L = NULL;

  assert(InitCList(L) == true);
  assert(L != NULL);
  assert(L->data == 1);      // 仅包含头节点
  assert(GetLength(L) == 1); // 验证 GetLength 函数

  printf("✅ 初始化与长度测试通过\n\n");
}

void TestInsert() {
  printf("--- 运行测试: 节点插入 ---\n");
  CDLinkList L;
  InitCList(L);

  // 此时只有头节点(index 0)。我们要在 index 1 之前插入，相当于尾插
  assert(InsertPrior(L, 1, 10) == true);
  assert(L->data == 2);

  // 在 index 2 之前插入
  assert(InsertPrior(L, 2, 20) == true);

  // 在 index 3 之前插入
  assert(InsertPrior(L, 3, 30) == true);

  // 现在的链表应该是: Head <-> 10 <-> 20 <-> 30
  PrintList(L);

  DLNode *p = GetElem(L, 1);
  assert(p->data == 10);
  p = GetElem(L, 3);
  assert(p->data == 30);

  printf("✅ 节点插入测试通过\n\n");
}

void TestSearchAndNegativeIndex() {
  printf("--- 运行测试: 查找与负数索引支持 ---\n");
  CDLinkList L;
  InitCList(L);
  InsertPrior(L, 1, 100);
  InsertPrior(L, 2, 200);
  InsertPrior(L, 3, 300);
  // 链表结构: Head(0) <-> 100(1) <-> 200(2) <-> 300(3)
  // 长度 L->data = 4

  // 1. 测试 LocateElem (按值查找)
  DLNode *p1 = LocateElem(L, 200);
  assert(p1 != NULL && p1->data == 200);
  DLNode *p2 = LocateElem(L, 999);
  assert(p2 == NULL); // 找不到

  // 2. 测试 GetElem 的正向索引
  assert(GetElem(L, 1)->data == 100);
  assert(GetElem(L, 3)->data == 300);

  // 3. 测试 GetElem 的负数索引
  assert(GetElem(L, -1)->data == 300); // -1 映射到最后一个元素
  assert(GetElem(L, -3)->data == 100); // -3 映射到第一个元素
  assert(GetElem(L, -4) == L);         // -4 映射到头节点
  assert(GetElem(L, -5)->data == 300); // 测试极端的负数越界循环

  printf("✅ 查找与负数索引测试通过\n\n");
}

void TestDelete() {
  printf("--- 运行测试: 节点删除 ---\n");
  CDLinkList L;
  InitCList(L);
  InsertPrior(L, 1, 10);
  InsertPrior(L, 2, 20);
  InsertPrior(L, 3, 30);
  // 链表: Head <-> 10 <-> 20 <-> 30, len=4

  int deleted_val = 0;

  // 删除第3个节点前面的那个节点 (即删除第2个节点, 数据为20)
  assert(DeletePrior(L, 3, deleted_val) == true);

  // 注意：要让下面这行 assert 通过，你需要将头文件中 DeletePrior 的参数改为
  // ElemType &val 并加上 val = p->data; assert(deleted_val == 20);

  assert(L->data == 3); // 长度减1

  // 现在的链表应该是: Head <-> 10 <-> 30
  PrintList(L);
  assert(GetElem(L, 2)->data == 30);

  // 尝试删除头节点 (index=1 时删除其前一个，即头节点)
  assert(DeletePrior(L, 1, deleted_val) == false);

  printf("✅ 节点删除测试通过\n\n");
}

int main() {
  printf("开始双向循环链表功能测试...\n\n");

  TestInitAndLength();
  TestInsert();
  TestSearchAndNegativeIndex();
  TestDelete();

  printf("🎉 所有测试用例均顺利通过！\n");
  return 0;
}
