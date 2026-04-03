#ifndef INCLUDE_TREE_BINARYTREE_H
#define INCLUDE_TREE_BINARYTREE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue/SqQueue.h"

typedef struct {
  void* arr;
  size_t elem_size;
  int capacity;
  int size;
} BiTreeArr;

typedef struct Node {
  Node* l;
  Node* r;
  char data[];
} Node, *BiTreeLi;

static inline void recrsize_free_node_(Node* root) {
  if (!root) return;
  recrsize_free_node_(root->l);
  recrsize_free_node_(root->r);
  free(root);
}

typedef struct {
  int (*is_valid)(void* tree, void* node_handle);
  void* (*get_left)(void* tree, void* node_handle);
  void* (*get_right)(void* tree, void* node_handle);
} TreeOps;

typedef void (*VisitFunc)(void* tree, void* node_handle);
typedef int (*Comparator)(const void*, const void*, size_t elem_size);

static inline void PreOrder(void* tree, void* root_handle, TreeOps ops,
                            VisitFunc visit) {
  if (!ops.is_valid(tree, root_handle)) return;
  visit(tree, root_handle);
  PreOrder(tree, ops.get_left(tree, root_handle), ops, visit);
  PreOrder(tree, ops.get_right(tree, root_handle), ops, visit);
}

static inline void InOrder(void* tree, void* root_handle, TreeOps ops,
                           VisitFunc visit) {
  if (!ops.is_valid(tree, root_handle)) return;
  InOrder(tree, ops.get_left(tree, root_handle), ops, visit);
  visit(tree, root_handle);
  InOrder(tree, ops.get_right(tree, root_handle), ops, visit);
}

static inline void PostOrder(void* tree, void* root_handle, TreeOps ops,
                             VisitFunc visit) {
  if (!ops.is_valid(tree, root_handle)) return;
  PostOrder(tree, ops.get_left(tree, root_handle), ops, visit);
  PostOrder(tree, ops.get_right(tree, root_handle), ops, visit);
  visit(tree, root_handle);
}

static inline void LevelOrder(void* tree, void* root_handle, TreeOps ops,
                              VisitFunc visit) {
  if (!ops.is_valid(tree, root_handle)) return;

  SqQueue* que = NULL;
  InitQueue(&que, sizeof(void*), 50);
  EnQueue(que, (const void*)&root_handle);
  while (!isSqQueueEmpty(que)) {
    void* head = NULL;
    DeQueue(que, (void*)&head);
    visit(tree, head);
    void* left = ops.get_left(tree, head);
    if (ops.is_valid(tree, left)) {
      EnQueue(que, (const void*)&left);
    }
    void* right = ops.get_right(tree, head);
    if (ops.is_valid(tree, right)) {
      EnQueue(que, (const void*)&right);
    }
  }
  DestorySqQueue(&que);
}

enum TreeError { TREE_OK = 0, TREE_ERR_MALLOC, TREE_ERR_INVALID_INPUT };

static inline void DestroyBiTreeLi(Node* node) {
  if (node == NULL) return;
  DestroyBiTreeLi(node->l);
  DestroyBiTreeLi(node->r);
  free(node);
}

static inline void* get_rootptr_(void* root, void* vec, size_t elem_size,
                                 uint len) {
  char* base = (char*)vec;
  for (uint i = 0; i < len; i++) {
    char* curr = base + i * elem_size;
    if (memcmp(root, (void*)curr, elem_size) == 0) {
      return curr;
    }
  }
  return NULL;
}

static inline BiTreeLi fromPreAndInOrderHelper_(void* pre, void* in,
                                                size_t elem_size, uint len,
                                                int* err) {
  if (*err != TREE_OK) return NULL;
  if (len == 0) return NULL;

  Node* rootNode = (Node*)malloc(sizeof(Node) + elem_size);
  if (rootNode == NULL) {
    *err = TREE_ERR_MALLOC;
    return NULL;
  }
  memcpy(rootNode->data, pre, elem_size);
  rootNode->l = NULL;
  rootNode->r = NULL;

  void* root_in_in = get_rootptr_(pre, in, elem_size, len);
  if (root_in_in == NULL) {
    *err = TREE_ERR_INVALID_INPUT;
    free(rootNode);
    return NULL;
  }

  void* right_in_in = (void*)((char*)root_in_in + elem_size);

  uint left_len = ((char*)root_in_in - (char*)in) / elem_size;
  uint right_len = len - left_len - 1;

  void* left_pre = (void*)((char*)pre + elem_size);
  void* right_pre = (void*)((char*)pre + (left_len + 1) * elem_size);

  BiTreeLi left_tree =
      fromPreAndInOrderHelper_(left_pre, in, elem_size, left_len, err);
  if (*err != TREE_OK) {
    free(rootNode);
    return NULL;
  }
  BiTreeLi right_tree = fromPreAndInOrderHelper_(right_pre, right_in_in,
                                                 elem_size, right_len, err);
  if (*err != TREE_OK) {
    DestroyBiTreeLi(rootNode->l);
    free(rootNode);
    return NULL;
  }

  rootNode->l = left_tree;
  rootNode->r = right_tree;
  return rootNode;
}

static inline BiTreeLi fromPreAndInOrder(void* pre, void* in, size_t elem_size,
                                         uint len) {
  int err = TREE_OK;
  BiTreeLi root = fromPreAndInOrderHelper_(pre, in, elem_size, len, &err);

  if (err != TREE_OK) {
    fprintf(stderr, "[ERROR] fromPreAndInOrder Failed! Error Code: %d\n", err);

    switch (err) {
      case TREE_ERR_MALLOC:
        fprintf(
            stderr,
            " -> Reason: Memory allocation failed (malloc returned NULL).\n");
        break;
      case TREE_ERR_INVALID_INPUT:
        fprintf(stderr,
                " -> Reason: Invalid input data. Root node not found in "
                "InOrder sequence.\n");
        fprintf(stderr,
                " -> Suggestion: Check if PreOrder and InOrder arrays match "
                "the same tree.\n");
        break;
      default:
        fprintf(stderr, " -> Reason: Unknown error.\n");
        break;
    }

    if (root != NULL) {
      fprintf(stderr,
              " -> Cleanup: Destroying partial tree to prevent leak.\n");
      DestroyBiTreeLi(root);
    }

    return NULL;
  }

  return root;
}
#endif  // !INCLUDE_TREE_BINARYTREE_H
