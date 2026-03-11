#ifndef LEARNCPP_PKGS_LINKLIST_DLINKLIST_H
#define LEARNCPP_PKGS_LINKLIST_DLINKLIST_H
#include <cstdbool>
#include <cstddef>
#include <cstdlib>

#define ElemType int
typedef struct DLNode {
  ElemType data;
  struct DLNode *prior;
  struct DLNode *next;
} DLNode, *DLinkList;

inline bool InitDList(DLinkList &L) {
  L = (DLNode *)std::malloc(sizeof(DLNode));
  if (L == NULL) {
    return false;
  }
  L->next = NULL;
  L->prior = NULL;
  L->data = 1;
  return true;
}

/**
 * @brief C++风格求链表长度(包含头节点)
 * * @return int
 */
inline int LengthWithHeadNode(DLinkList L) {
  DLNode *p = L;
  int len = 0;
  while (p != NULL) {
    len++;
    p = p->next;
  }
  return len;
}

/**
 * @brief C++风格求链表长度(只计算数据节点长度，不包含头节点)
 * * @return int
 *    - 返回-1表示传入的头指针是NULL,即该链表不存在
 */
inline int LengthWithoutHeadNode(DLinkList L) {
  DLNode *p = L;
  int len = 0;
  while (p != NULL) {
    len++;
    p = p->next;
  }
  return len - 1;
}

/**
 * @brief C++风格按索引查找节点
 * - 传入的链表应包含头节点，规定头节点索引为0，数据节点的索引从1开始
 * * @return LNode*
 *    - 返回携带该值的链表节点的指针
 */
inline DLNode *GetElem(DLinkList L, uint index) {
  DLNode *p = L;
  uint j = 0;
  while (p != NULL && j < index) {
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
inline DLNode *LocateElem(DLinkList L, ElemType val) {
  if (L == NULL)
    return NULL;
  DLNode *p =
      L->next; // headnote存储的数据语义和DataNode的语义完全不同，须手动跳过
  while (p != NULL && p->data != val) {
    p = p->next;
  }
  return p;
}

/**
 * @brief L包含头节点，在第i个数据节点之前插入一个节点
 */
inline bool InsertPrior(DLinkList &L, uint index, ElemType val) {
  if (index < 1) {
    return false;
  }

  DLNode *p = GetElem(L, index - 1);
  DLNode *q = (DLNode *)malloc(sizeof(DLNode));
  if (p == NULL || q == NULL) {
    return false;
  }

  q->data = val;

  q->next = p->next;
  q->prior = p;
  p->next = q;
  if (q->next != NULL) {
    q->next->prior = q;
  }

  L->data++; // update list length stored in HdadNode data
  return true;
}

/**
 * @brief L包含头节点，删除第i个数据节点之的前一个节点
 */
inline bool DeletePrior(DLinkList &L, uint index, ElemType val) {
  if (index < 2) {
    return false;
  }

  DLNode *p = GetElem(L, index - 1);
  if (p == NULL) {
    return false;
  }

  p->prior->next = p->next;
  if (p->next != NULL) {
    p->next->prior = p->prior;
  }
  free(p);

  L->data--; // update list length stored in HdadNode data
  return true;
}

#endif // !LEARNCPP_PKGS_LINKLIST_DLINKLIST_H
