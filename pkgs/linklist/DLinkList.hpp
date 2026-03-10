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
  return true;
}

#endif // !LEARNCPP_PKGS_LINKLIST_DLINKLIST_H
