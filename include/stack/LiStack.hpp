/**
 * impl a `stack` with headless linked-list
 */
#ifndef INCLUDE_STACKLISTACK_HPP
#define INCLUDE_STACKLISTACK_HPP
#include <cstddef>
#include <cstdlib>

#define ElemType int

typedef struct LinkNode {
  ElemType data;
  LinkNode* next;
} LinkNode, *LiStack;

inline bool isStackEmpty(LiStack s) {
  if (s == NULL) {
    return true;
  } else {
    return false;
  }
}

inline bool Push(LiStack* s, int val) {
  LinkNode* new_node = (LinkNode*)malloc(sizeof(LinkNode));
  if (new_node == NULL) {
    return false;
  }
  new_node->next = *s;
  new_node->data = val;
  *s = new_node;
  return true;
}

inline bool Pop(LiStack* s, ElemType* val) {
  if (s == NULL || *s == NULL) {
    return false;
  }
  *val = (*s)->data;
  LinkNode* p = (*s);
  (*s) = (*s)->next;
  free(p);
  return true;
}

inline bool GetTop(LiStack s, ElemType* val) {
  if (s == NULL) {
    return false;
  }
  *val = s->data;
  return true;
}

inline bool DestroyStack(LiStack* s) {
  if (s == NULL || *s == NULL) {
    return false;
  }
  LinkNode* p = *s;
  while ((*s) != NULL) {
    (*s) = (*s)->next;
    free(p);
    p = *s;
  }
  return true;
}
#endif  // !INCLUDE_STACKLISTACK_HPP
