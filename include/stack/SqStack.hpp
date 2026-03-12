#ifndef INCLUDE_STACK_SQSTACK_HPP
#define INCLUDE_STACK_SQSTACK_HPP
#include <asm-generic/errno.h>

#include <cstddef>
#include <cstdlib>

#define ElemType int

typedef struct {
  ElemType* data;
  int capacity;
  int top;
} SqStack;

inline bool InitStack(SqStack** s, int capacity = 500) {
  ElemType* data = (ElemType*)malloc(sizeof(ElemType) * capacity);
  if (data == NULL) {
    return false;
  }
  *s = (SqStack*)malloc(sizeof(SqStack));
  if (*s == NULL) {
    free(data);
    return false;
  }

  (*s)->capacity = capacity;
  (*s)->data = data;
  (*s)->top = -1;
  return true;
}

inline bool DestroyStack(SqStack** s) {
  if (s == NULL || *s == NULL) {
    return false;
  }
  if ((*s)->data != NULL) {
    free((*s)->data);
  }
  free(*s);
  s = NULL;
  return true;
}

inline bool isStackEmpty(SqStack* s) {
  if (s == NULL) {
    return true;
  }
  if (s->top == -1) {
    return true;
  } else {
    return false;
  }
}

inline bool Push(SqStack* s, ElemType val) {
  if (s == NULL || s->top + 1 > s->capacity - 1) {
    return false;
  }
  s->data[++s->top] = val;
  return true;
}

inline bool Pop(SqStack* s, ElemType* val) {
  if (isStackEmpty(s)) {
    return false;
  }
  *val = s->data[s->top];
  s->top--;
  return true;
}

inline bool GetTop(SqStack* s, ElemType* val) {
  if (isStackEmpty(s)) {
    return false;
  }
  *val = s->data[s->top];
  return true;
}
#endif  // !INCLUDE_STACK_SQSTACK_HPP
