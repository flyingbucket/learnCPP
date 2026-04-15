#ifndef INCLUDE_TREE_AVLTREE_H
#define INCLUDE_TREE_AVLTREE_H
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wunused-function"
#elif defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wunused-function"
#endif

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus
#include <stddef.h>

#include "tree/BinaryTree.h"

typedef struct AVLNode {
  struct AVLNode* l;
  struct AVLNode* r;
  int h;
  char data[];
} AVLNode;
typedef struct {
  AVLNode* root;
  Comparator cmp;
} AVLTree;

static inline AVLNode* rotate_right(AVLNode* y) {
  AVLNode* x = y->l;
  y->l = x->r;
  x->r = y;
  // 更新高度
  int yl = y->l ? y->l->h : 0, yr = y->r ? y->r->h : 0;
  y->h = (yl > yr ? yl : yr) + 1;
  int xl = x->l ? x->l->h : 0, xr = x->r ? x->r->h : 0;
  x->h = (xl > xr ? xl : xr) + 1;
  return x;
}

static inline AVLNode* rotate_left(AVLNode* x) {
  AVLNode* y = x->r;
  x->r = y->l;
  y->l = x;
  // 更新高度
  int xl = x->l ? x->l->h : 0, xr = x->r ? x->r->h : 0;
  x->h = (xl > xr ? xl : xr) + 1;
  int yl = y->l ? y->l->h : 0, yr = y->r ? y->r->h : 0;
  y->h = (yl > yr ? yl : yr) + 1;
  return y;
}
static inline bool AVLTreeInsert(AVLTree* tree, const char* key,
                                 size_t elem_size) {
  if (!tree) return false;

  AVLNode** path[64];
  int top = 0;

  AVLNode** curr = &(tree->root);
  while (*curr) {
    path[top++] = curr;  // 记录路径
    int res = tree->cmp(key, (*curr)->data, elem_size);
    if (res == 0) return false;  // 重复键
    curr = (res > 0) ? &((*curr)->r) : &((*curr)->l);
  }

  // 插入新节点
  AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode) + elem_size);
  if (!node) return false;
  node->l = node->r = NULL;
  node->h = 1;  // 新叶子高度为 1
  memcpy(node->data, key, elem_size);
  *curr = node;

  // 向上回溯并旋转
  while (top > 0) {
    AVLNode** p_node_ptr = path[--top];
    AVLNode* p = *p_node_ptr;

    // 更新当前节点高度
    int old_h = p->h;
    int hl = p->l ? p->l->h : 0;
    int hr = p->r ? p->r->h : 0;
    p->h = (hl > hr ? hl : hr) + 1;

    // 计算平衡因子 (BF)
    int bf = hl - hr;

    // 如果不平衡，执行旋转逻辑
    if (bf > 1 || bf < -1) {
      // --- 旋转逻辑开始 ---
      if (bf > 1) {  // L 型
        int l_hl = p->l->l ? p->l->l->h : 0;
        int l_hr = p->l->r ? p->l->r->h : 0;
        if (l_hl >= l_hr) {  // LL 旋转
          *p_node_ptr = rotate_right(p);
        } else {  // LR 旋转
          p->l = rotate_left(p->l);
          *p_node_ptr = rotate_right(p);
        }
      } else {  // R 型
        int r_hl = p->r->l ? p->r->l->h : 0;
        int r_hr = p->r->r ? p->r->r->h : 0;
        if (r_hr >= r_hl) {  // RR 旋转
          *p_node_ptr = rotate_left(p);
        } else {  // RL 旋转
          p->r = rotate_right(p->r);
          *p_node_ptr = rotate_left(p);
        }
      }
      // 旋转后该子树高度已平衡，AVL 插入只需旋转一次即可停止回溯
      break;
    }

    // 如果高度没变，说明上方也不需要调整，直接退出
    if (p->h == old_h) break;
  }

  return true;
}

static inline void InitAVLTree(AVLTree* tree, Comparator cmp) {
  if (!tree) return;
  tree->cmp = cmp;
  tree->root = NULL;
}

static inline void recrsize_free_AVLNode_(AVLNode* root) {
  if (!root) return;
  recrsize_free_AVLNode_(root->l);
  recrsize_free_AVLNode_(root->r);
  free(root);
}
static inline void DestroyAVLTree(AVLTree* tree) {
  if (!tree) return;
  recrsize_free_AVLNode_(tree->root);
  tree->root = NULL;
}

#ifdef __cplusplus
}
#endif  // __cplusplus

#if defined(__clang__)
#pragma clang diagnostic warning "-Wunused-function"
#elif defined(__GNUC__)
#pragma GCC diagnostic warning "-Wunused-function"
#endif

#endif  // !INCLUDE_TREE_AVLTREE_H
