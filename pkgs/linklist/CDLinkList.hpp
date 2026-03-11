#ifndef LEARNCPP_PKGS_LINKLIST_CLINKLIST_H
#define LEARNCPP_PKGS_LINKLIST_CLINKLIST_H
#include <cassert>
#include <cstdbool>
#include <cstddef>
#include <cstdlib>

#define ElemType int
typedef struct DLNode {
  ElemType data;
  struct DLNode *prior;
  struct DLNode *next;
} DLNode, *CDLinkList;

inline bool InitCList(CDLinkList &L) {
  L = (DLNode *)std::malloc(sizeof(DLNode));
  if (L == NULL) {
    return false;
  }
  L->next = L;
  L->prior = L;
  L->data = 1;
  return true;
}

/**
 * @brief C++风格求链表长度(包含头节点)
 * * @return int
 */
inline int GetLength(CDLinkList L) {
  if (L == NULL)
    return 0;
  DLNode *p = L->next;
  int len = 1;
  while (p != L) {
    len++;
    p = p->next;
  }
  return len;
}

/**
 * @brief C++风格按索引查找节点
 * - 传入的链表应包含头节点，规定头节点索引为0，数据节点的索引从1开始
 * * @return LNode*
 *    - 返回携带该值的链表节点的指针
 */
inline DLNode *GetElem(CDLinkList L, int index) {

  if (L == NULL)
    return NULL;

  // get length of list from head node
  int len = L->data;
#ifndef NDEBUG
  // 仅在调试模式下校验长度，确保开发时能及时发现 Bug
  int actual_len = 1;
  DLNode *temp = L->next;
  while (temp != L) {
    actual_len++;
    temp = temp->next;
  }
  assert(actual_len == L->data && "警告：链表头节点长度数据与实际不符！");
#endif

  // enable negative index
  index = (index % len + len) % len;
  if (index == 0) {
    return L;
  }

  DLNode *p = L->next;
  int j = 1;
  while (p != L && j < index) {
    p = p->next;
    j++;
  }
  return p;
}

/**
 * @brief C++风格按值查找节点
 * @return LNode*
 *  - 返回携带该值的链表节点的指针
 */
inline DLNode *LocateElem(CDLinkList L, ElemType val) {
  if (L == NULL || L->next == L)
    return NULL;
  DLNode *p =
      L->next; // headnote存储的数据语义和DataNode的语义完全不同，须手动跳过
  while (p != L && p->data != val) {
    p = p->next;
  }
  if (p == L) {
    return NULL;
  }
  return p;
}

/**
 * @brief L包含头节点，在第i个节点之前插入一个节点
 */
inline bool InsertPrior(CDLinkList &L, int index, ElemType val) {
  if (L == NULL) {
    return false;
  }
  DLNode *p = GetElem(L, index - 1);
  DLNode *q = (DLNode *)malloc(sizeof(DLNode));
  if (q == NULL) {
    return false;
  }

  q->data = val;

  q->next = p->next;
  q->prior = p;
  p->next = q;
  q->next->prior = q;

  L->data++; // update list length stored in HdadNode data
  return true;
}

/**
 * @brief L包含头节点，删除第i个数据节点之的前一个节点
 */
inline bool DeletePrior(CDLinkList &L, int index, ElemType &val) {
  if (L == NULL || index == 1) { // deleting head node is not allowed
    return false;
  }
  int len = L->data;
  index = (index % len + len) % len;

  if (index == 1) { // deleting head node is not allowed
    return false;
  }

  DLNode *p = GetElem(L, index - 1);

  p->prior->next = p->next;
  p->next->prior = p->prior;
  val = p->data;
  free(p);

  L->data--; // update list length stored in HdadNode data
  return true;
}

#endif // !LEARNCPP_PKGS_LINKLIST_CLINKLIST_H
