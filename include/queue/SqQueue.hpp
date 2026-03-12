#ifndef INCLUDE_SQQUEUE_HPP
#define INCLUDE_SQQUEUE_HPP
#include <cstddef>
#include <cstdlib>

#define ElemType int
#define MaxSize 50

/**
 * @brief 循环队列,队尾后空出一个节点用于解决空队、满队判别
 */
typedef struct {
  ElemType data[MaxSize];
  int front, rear;
} SqQueue;

/**
 * @brief 初始化队列
 */
inline bool InitQueue(SqQueue **q) {
  *q = (SqQueue *)malloc(sizeof(SqQueue));
  if (*q == NULL) {
    return false;
  }
  (*q)->front = 0;
  (*q)->rear = 0;
  return true;
}

/**
 * @brief 循环队列 空队判别
 */
inline bool isSqQueueEmpty(SqQueue *q) {
  if (q->front == q->rear) {
    return true;
  } else {
    return false;
  }
}
/**
 * @brief 循环队列 满队判别
 */
inline bool isSqQueueFull(SqQueue *q) {
  int rear_plus_one = (q->rear + 1 + MaxSize) % MaxSize;
  if (q->front == rear_plus_one) {
    return true;
  } else {
    return false;
  }
}

/**
 * @brief 循环队列 入队
 */
inline bool EnQueue(SqQueue *q, ElemType val) {
  if (isSqQueueFull(q)) {
    return false;
  }

  q->data[q->rear] = val;
  q->rear = (q->rear + 1 + MaxSize) % MaxSize;
  return true;
}

/**
 * @brief 循环队列 出队
 */
inline bool DeQueue(SqQueue *q, ElemType *val) {
  if (isSqQueueEmpty(q)) {
    return false;
  }
  *val = q->data[q->front];
  q->front = (q->front + 1 + MaxSize) % MaxSize;
  return true;
}

inline int SqQueueLength(SqQueue *q) {
  return (q->rear - q->front + MaxSize) % MaxSize;
}
#endif // !INCLUDE_SQQUEUE_HPP
