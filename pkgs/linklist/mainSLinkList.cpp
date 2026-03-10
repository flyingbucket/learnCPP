#include "LinkListUtils.hpp"
#include "SLinkList.hpp"

int main() {
  printf("=== 开始链表边界测试 ===\n");

  // 1. 初始化测试
  LinkList L = NULL;
  InitList(L);
  printf("1. 初始化后长度 (不含头): %d\n", LengthWithoutHeadNode(L));

  // 2. 插入测试 (边界情况)
  printf("\n--- 插入测试 ---\n");
  ListInsert(L, 1, 10); // 在第1位插入 (首个数据节点)
  ListInsert(L, 2, 20); // 在第2位插入 (尾部插入)
  ListInsert(L, 1, 5);  // 再次在第1位插入 (头部挤兑)
  PrintList(L);         // 预期: Head -> [5] -> [10] -> [20] -> NULL

  // 3. 越界插入测试
  if (!ListInsert(L, 10, 99)) {
    printf("3. 越界插入捕捉成功 (Index 10)\n");
  }

  // 4. 查找测试
  printf("\n--- 查找测试 ---\n");
  LNode *found = GetElem(L, 2);
  if (found)
    printf("GetElem(2): %d\n", found->data); // 应该是 10

  LNode *loc = LocateElem(L, 20);
  if (loc)
    printf("LocateElem(20): 找到了地址 %p\n", (void *)loc);

  // 5. 删除测试 (边界情况)
  printf("\n--- 删除测试 ---\n");
  ListDelete(L, 1); // 删除第1个 [5]
  printf("删除第1个后: ");
  PrintList(L);

  ListDelete(L, 2); // 删除当前的第2个 (尾部)
  printf("删除尾部后: ");
  PrintList(L);

  // 6. 异常删除测试
  if (!ListDelete(L, 0))
    printf("6. 非法索引 0 删除拒绝成功\n");
  if (!ListDelete(L, 5))
    printf("6. 越界删除拒绝成功\n");

  // 7. 清理内存 (重要)
  // 实际项目中建议写一个 DestroyList 函数，这里简单演示
  while (L->next)
    ListDelete(L, 1);
  std::free(L);
  printf("\n=== 测试完成，内存已清理 ===\n");

  return EXIT_SUCCESS;
}
