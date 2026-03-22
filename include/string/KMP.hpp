#ifndef INCLUDE_STRING_KMP_HPP
#define INCLUDE_STRING_KMP_HPP
#include <alloca.h>

#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "basicOps.hpp"

inline void get_next_dummy(String* P, int next[]) {
  next[0] = -1;
  for (int i = 1; P->ch[i] != '\0'; i++) {
    int lsp = 0;
    for (int j = 0; j < i; j++) {
      int ok = 1;
      for (int k = 0; k <= j; k++) {
        if (P->ch[k] != P->ch[i - j + k]) {
          ok = 0;
          break;
        }
      }
      if (ok) {
        lsp = j;
      }
    }
    next[i] = lsp;
  }
}

/**
 * @brief 计算 KMP 算法模式串的 next 数组 (动态规划思想)
 *
 * `next[i]` 记录了模式串 `P[0...i-1]` 的最长相等前后缀的长度。
 * 利用已知的 `next[i]` = j，通过比较 `P[i]` 和 `P[j]` 递推计算 `next[i+1]` 。
 *
 * @param P 指向模式串结构体的指针
 * @param next 用于存储计算结果的数组（需由调用方保证长度大于等于 P->len）
 *
 * @note 核心回退逻辑 `j = next[j]` 的含义：
 * 当 `P[i] != P[j]` 失配时，等价于在前缀 `P[0...j-1]`
 * 内部寻找次长的相等前后缀， 该次长长度恰好已保存在 `next[j]` 中。
 */
inline void get_next(String* P, int next[]) {
  next[0] = -1;
  int i = 0, j = -1;
  while (i < P->len - 1) {
    if (j == -1 || P->ch[i] == P->ch[j]) {
      i++;
      j++;
      next[i] = j;
    } else {
      j = next[j];
    }
  }
}
inline int KMP(String* S, String* P) {
  if (S == NULL || P == NULL || S->len == 0 || P->len == 0) {
    return -1;
  }
  // int next[P->len];
  int* next = (int*)alloca(sizeof(int) * P->len);
  get_next(P, next);
  int i = 0, j = 0;
  while (i < S->len && j < P->len) {
    if (j == -1 || S->ch[i] == P->ch[j]) {
      i++;
      j++;
    } else {
      j = next[j];
    }
    if (j == P->len) {
      return i - P->len;
    }
  }
  return -1;
}
#endif  // !INCLUDE_STRING_KMP_HPP
