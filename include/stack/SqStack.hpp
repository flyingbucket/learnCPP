#ifndef INCLUDE_STACK_SQSTACK_HPP
#define INCLUDE_STACK_SQSTACK_HPP
#include <asm-generic/errno.h>

#include <cstddef>
#include <cstdlib>
#include <cstring>

// #define ElemType int

typedef struct {
  void* data;
  size_t elem_size;
  int capacity;
  int top;
} SqStack;

inline bool InitStack(SqStack** s, size_t elem_size, int capacity = 500) {
  void* data = (void*)malloc(elem_size * capacity);
  if (data == NULL) {
    return false;
  }
  *s = (SqStack*)malloc(sizeof(SqStack));
  if (*s == NULL) {
    free(data);
    return false;
  }

  (*s)->elem_size = elem_size;
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
  *s = NULL;
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

inline bool Push(SqStack* s, const void* val) {
  if (s == NULL || s->top + 1 > s->capacity - 1) {
    return false;
  }
  void* target = (char*)s->data + (++s->top * s->elem_size);
  memcpy(target, val, s->elem_size);
  return true;
}

inline bool Pop(SqStack* s, void* val) {
  if (isStackEmpty(s)) {
    return false;
  }
  void* source = (char*)s->data + (s->elem_size * s->top--);
  memcpy(val, source, s->elem_size);
  return true;
}

inline bool GetTop(SqStack* s, void* val) {
  if (isStackEmpty(s)) {
    return false;
  }
  void* source = (char*)s->data + (s->elem_size * s->top);
  memcpy(val, source, s->elem_size);
  return true;
}
#endif  // !INCLUDE_STACK_SQSTACK_HPP
