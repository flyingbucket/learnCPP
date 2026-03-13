#ifndef INCLUDE_SQQUEUE_HPP
#define INCLUDE_SQQUEUE_HPP
#include <cstddef>
#include <cstdlib>
#include <cstring>

/**
 * @brief 循环队列,队尾后空出一个节点用于解决空队、满队判别
 */
typedef struct {
  void* data;
  size_t elem_size;
  int capacity;
  int front, rear;
} SqQueue;

/**
 * @brief 初始化队列
 */
inline bool InitQueue(SqQueue** q, size_t elem_size, int capacity = 50) {
  void* data = (void*)malloc(elem_size * capacity);
  *q = (SqQueue*)malloc(sizeof(SqQueue));
  if (data == NULL || *q == NULL) {
    return false;
  }
  (*q)->elem_size = elem_size;
  (*q)->capacity = capacity;
  (*q)->data = data;
  (*q)->front = 0;
  (*q)->rear = 0;
  return true;
}

/**
 * @brief 循环队列 空队判别
 */
inline bool isSqQueueEmpty(SqQueue* q) {
  if (q->front == q->rear) {
    return true;
  } else {
    return false;
  }
}
/**
 * @brief 循环队列 满队判别
 */
inline bool isSqQueueFull(SqQueue* q) {
  int capacity = q->capacity;
  int rear_plus_one = (q->rear + 1) % capacity;
  if (q->front == rear_plus_one) {
    return true;
  } else {
    return false;
  }
}

/**
 * @brief 循环队列 入队
 */
inline bool EnQueue(SqQueue* q, const void* val) {
  if (isSqQueueFull(q)) {
    return false;
  }
  int capacity = q->capacity;
  void* target = (char*)q->data + (q->elem_size * q->rear);
  memcpy(target, val, q->elem_size);
  q->rear = (q->rear + 1 + capacity) % capacity;
  return true;
}

/**
 * @brief 循环队列 出队
 */
inline bool DeQueue(SqQueue* q, void* val) {
  if (isSqQueueEmpty(q)) {
    return false;
  }
  int capacity = q->capacity;
  void* source = (char*)q->data + (q->front * q->elem_size);
  memcpy(val, source, q->elem_size);
  q->front = (q->front + 1 + capacity) % capacity;
  return true;
}

inline int SqQueueLength(SqQueue* q) {
  int capacity = q->capacity;
  return (q->rear - q->front + capacity) % capacity;
}

inline bool DestorySqQueue(SqQueue** q) {
  if (q == NULL || *q == NULL || (*q)->data == NULL) {
    return false;
  }
  free((*q)->data);
  free(*q);
  *q = NULL;
  return true;
}
#endif  // !INCLUDE_SQQUEUE_HPP
