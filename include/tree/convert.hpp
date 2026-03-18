#ifndef INCLUDE_TREE_CONVERT_HPP
#define INCLUDE_TREE_CONVERT_HPP
#include <cstddef>
#include <cstdlib>

#include "tree/BinaryTree.hpp"

typedef struct GeneralTree {
  void* data;
  struct GeneralTree** childs;
  int childs_count;
  int childs_capacity;
} GeneralTree;

inline BiTreeLi GeneralTree2BiTree(GeneralTree* tree) {
  if (tree == NULL) return NULL;
  Node* bin_node = (Node*)malloc(sizeof(Node));
  bin_node->data = tree->data;
  bin_node->l = NULL;
  bin_node->r = NULL;

  if (tree->childs_count != 0 && tree->childs != NULL) {
    bin_node->l = GeneralTree2BiTree(tree->childs[0]);
    Node* current_sibling = bin_node->l;
    for (int i = 1; i < tree->childs_count; i++) {
      Node* next_sibling = GeneralTree2BiTree(tree->childs[i]);
      current_sibling->r = next_sibling;
      current_sibling = next_sibling;
    }
  }
  return bin_node;
}

#endif  // !INCLUDE_TREE_CONVERT_HPP
