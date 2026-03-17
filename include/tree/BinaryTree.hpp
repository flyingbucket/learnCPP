#ifndef INCLUDE_TREE_BINARYTREE_HPP
#define INCLUDE_TREE_BINARYTREE_HPP
#include <cstdint>
#include <cstdlib>

typedef struct {
  void* arr;
  size_t elem_size;
  int capacity;
  int size;
} BiTreeArr;

typedef struct Node {
  void* data;
  Node* l;
  Node* r;
} Node, *BiTreeLi;

typedef struct {
  int (*is_valid)(void* tree, void* node_handle);
  void* (*get_left)(void* tree, void* node_handle);
  void* (*get_right)(void* tree, void* node_handle);
} TreeOps;

typedef void (*VisitFunc)(void* tree, void* node_handle);

inline void PreOrder(void* tree, void* root_handle, TreeOps ops,
                     VisitFunc visit) {
  if (!ops.is_valid(tree, root_handle)) return;
  visit(tree, root_handle);
  PreOrder(tree, ops.get_left(tree, root_handle), ops, visit);
  PreOrder(tree, ops.get_right(tree, root_handle), ops, visit);
}

inline void InOrder(void* tree, void* root_handle, TreeOps ops,
                    VisitFunc visit) {
  if (!ops.is_valid(tree, root_handle)) return;
  InOrder(tree, ops.get_left(tree, root_handle), ops, visit);
  visit(tree, root_handle);
  InOrder(tree, ops.get_right(tree, root_handle), ops, visit);
}

inline void PostOrder(void* tree, void* root_handle, TreeOps ops,
                      VisitFunc visit) {
  if (!ops.is_valid(tree, root_handle)) return;
  PostOrder(tree, ops.get_left(tree, root_handle), ops, visit);
  PostOrder(tree, ops.get_right(tree, root_handle), ops, visit);
  visit(tree, root_handle);
}

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

#endif  // !INCLUDE_TREE_BINARYTREE_HPP
