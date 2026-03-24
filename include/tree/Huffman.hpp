#ifndef INCLUDE_TREE_HUFFMAN_HPP
#define INCLUDE_TREE_HUFFMAN_HPP
#include <cstddef>
#include <cstdlib>

#include "BinaryTree.hpp"
#include "queue/SqQueue.h"

inline int compare_ints_asc_(const void* a, const void* b) {
  int val_a = *(const int*)a;
  int val_b = *(const int*)b;
  if (val_a < val_b) return -1;
  if (val_a > val_b) return 1;
  return 0;
}

inline Node* get_next_node_(SqQueue* ori, SqQueue* sum) {
  Node* ori_node = NULL;
  Node* sum_node = NULL;
  int ori_w = -1, sum_w = -1;

  if (GetFront(ori, (void*)&ori_node) && ori_node != NULL) {
    ori_w = *(int*)(ori_node->data);
  }
  if (GetFront(sum, (void*)&sum_node) && sum_node != NULL) {
    sum_w = *(int*)(sum_node->data);
  }

  Node* selected_node = NULL;

  if (ori_w >= 0 && sum_w >= 0) {
    if (ori_w <= sum_w) {
      DeQueue(ori, (void*)&selected_node);
    } else {
      DeQueue(sum, (void*)&selected_node);
    }
  } else if (ori_w >= 0) {
    DeQueue(ori, (void*)&selected_node);
  } else if (sum_w >= 0) {
    DeQueue(sum, (void*)&selected_node);
  }

  return selected_node;
}
inline BiTreeLi BuildHuffman(int* w, size_t len) {
  qsort(w, len, sizeof(int), compare_ints_asc_);
  SqQueue* ori = NULL;
  SqQueue* sum = NULL;
  bool queue_tag = true;
  queue_tag = InitQueue(&ori, sizeof(Node*), 50);
  if (!queue_tag) return NULL;
  queue_tag = InitQueue(&sum, sizeof(Node*), 50);
  if (!queue_tag) {
    DestorySqQueue(&ori);
    return NULL;
  }

  for (size_t i = 0; i < len; i++) {
    Node* new_node = (Node*)malloc(sizeof(Node) + sizeof(int));
    new_node->l = NULL;
    new_node->r = NULL;
    *(int*)(new_node->data) = w[i];
    queue_tag = EnQueue(ori, (void*)&new_node);
  }

  while (SqQueueLength(ori) + SqQueueLength(sum) > 1) {
    Node* new_root = (Node*)malloc(sizeof(Node) + sizeof(int));
    new_root->l = NULL;
    new_root->r = NULL;
    Node* new_l = get_next_node_(ori, sum);
    Node* new_r = get_next_node_(ori, sum);
    if (new_l == NULL || new_r == NULL) queue_tag = false;
    *(int*)(new_root->data) = *(int*)(new_l->data) + *(int*)(new_r->data);
    new_root->l = new_l;
    new_root->r = new_r;
    EnQueue(sum, (void*)&new_root);
  }

  Node* huffman = NULL;
  if (!isSqQueueEmpty(sum)) {
    queue_tag = DeQueue(sum, (void*)&huffman);
  } else if (!isSqQueueEmpty(ori)) {
    queue_tag = DeQueue(ori, (void*)&huffman);
  }

  DestorySqQueue(&ori);
  DestorySqQueue(&sum);
  if (!queue_tag) return NULL;
  return huffman;
}
#endif  // !INCLUDE_TREE_HUFFMAN_HPP
