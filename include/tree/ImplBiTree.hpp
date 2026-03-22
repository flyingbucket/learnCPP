#ifndef INCLUDE_TREE_INPLBITREE_HPP
#define INCLUDE_TREE_INPLBITREE_HPP
#include <cstdint>

#include "BinaryTree.hpp"

inline int li_is_valid(void* tree, void* node) { return node != NULL; }
inline void* li_get_left(void* ctx, void* node) { return ((Node*)node)->l; }

inline void* li_get_right(void* ctx, void* node) { return ((Node*)node)->r; }

inline void li_visit_int(void* ctx, void* node) {
  int val = *(int*)(((Node*)node)->data);
  printf("%d ", val);
}

const TreeOps BiTreeLiOps = {
    .is_valid = li_is_valid,
    .get_left = li_get_left,
    .get_right = li_get_right,
};

inline int arr_is_valid(void* tree, void* node) {
  BiTreeArr* arTree = (BiTreeArr*)tree;
  intptr_t index = (intptr_t)node;
  return index < arTree->size;
}

inline void* arr_get_left(void* tree, void* node) {
  BiTreeArr* arTree = (BiTreeArr*)tree;
  intptr_t index = (intptr_t)node;
  return (void*)(index * 2 + 1);
}

inline void* arr_get_right(void* tree, void* node) {
  BiTreeArr* arTree = (BiTreeArr*)tree;
  intptr_t index = (intptr_t)node;
  return (void*)(index * 2 + 2);
}

inline void arr_visit_int(void* tree, void* node) {
  BiTreeArr* arTree = (BiTreeArr*)tree;
  intptr_t index = (intptr_t)node;
  char* target = (char*)arTree->arr + index * arTree->elem_size;
  printf("%d", *(int*)target);
}

const TreeOps BiTreeArrOps = {
    .is_valid = arr_is_valid,
    .get_left = arr_get_left,
    .get_right = arr_get_right,
};
#endif  // !INCLUDE_TREE_INPLBITREE_HPP
