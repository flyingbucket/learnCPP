#ifndef INCLUDE_TREE_BST_H
#define INCLUDE_TREE_BST_H

#include <stdbool.h>
#include <stdlib.h>

#include "tree/BinaryTree.h"

typedef struct {
  Node* root;
  Comparator cmp;
} BST;

static inline void InitBST(BST* tree, Comparator cmp) {
  if (!tree) return;
  tree->cmp = cmp;
  tree->root = NULL;
}
static inline void DestroyBST(BST* tree) {
  if (tree && tree->root) {
    recrsize_free_node_(tree->root);
    tree->root = NULL;
  }
}

static inline Node* BSTfind(const BST* tree, const char* key,
                            size_t elem_size) {
  Node* curr = tree->root;
  while (curr != NULL) {
    int res = tree->cmp(key, curr->data, elem_size);
    if (res == 0) return curr;
    curr = (res > 0) ? curr->r : curr->l;
  }
  return NULL;
}

static inline bool BSTinsert(BST* tree, const char* key, size_t elem_size) {
  Node** curr = &(tree->root);
  while (*curr) {
    int res = tree->cmp(key, (*curr)->data, elem_size);
    if (res == 0) return false;
    curr = (res > 0) ? &(*curr)->r : &(*curr)->l;
  }
  Node* node = (Node*)malloc(sizeof(Node) + elem_size);
  if (!node) return false;
  memcpy(node->data, key, elem_size);
  node->l = NULL;
  node->r = NULL;
  *curr = node;
  return true;
}

static inline bool BSTdelete(BST* tree, const char* key, size_t elem_size) {
  if (!tree || !tree->root) return false;

  // find target
  Node** curr = &(tree->root);
  while (*curr) {
    int res = tree->cmp(key, (*curr)->data, elem_size);
    if (res == 0) break;  // found
    curr = (res > 0) ? &(*curr)->r : &(*curr)->l;
  }

  if (*curr == NULL) return false;  // not found

  Node* old_node = *curr;

  // delete
  if (old_node->l == NULL) {
    // left child NULL, right child whatever
    *curr = old_node->r;
  } else if (old_node->r == NULL) {
    // right child NULL, left child whatever
    *curr = old_node->l;
  } else {
    // both not NULL
    Node** successor = &(old_node->r);
    while ((*successor)->l) {
      successor = &((*successor)->l);
    }

    Node* s_node = *successor;

    // 让后继节点的父节点接管后继节点的右子树
    *successor = s_node->r;

    // 将后继节点替换到当前位置
    s_node->l = old_node->l;
    s_node->r = old_node->r;
    *curr = s_node;
  }

  free(old_node);
  return true;
}
#endif  // !INCLUDE_TREE_BST_H
