#ifndef LINKED_LIST_UTILS_HPP
#define LINKED_LIST_UTILS_HPP

#include <concepts>
#include <type_traits>

namespace ListUtils {

/**
 * @brief 约束：类型 T 必须是一个指向包含 next 成员的结构体的指针
 */
template <typename T>
concept IsLinkedNode = requires(T p) {
  { p->next } -> std::convertible_to<T>;
};

/**
 * @brief 约束：类型 T 必须包含 data 成员，且能与 ValType 进行比较
 */
template <typename T, typename ValType>
concept HasDataAndComparable = requires(T p, ValType val) {
  { p->data } -> std::equality_comparable_with<ValType>;
};

/**
 * @brief 通用链表长度计算
 */
template <IsLinkedNode NodePtr> inline int GetLength(NodePtr head) {
  int len = 0;
  NodePtr p = head;
  while (p != nullptr) {
    len++;
    p = p->next;
  }
  return len;
}

/**
 * @brief 按索引查找节点
 */
template <IsLinkedNode NodePtr, std::integral IndexType>
inline NodePtr GetElem(NodePtr head, IndexType index) {
  NodePtr p = head;
  IndexType j = 0;
  while (p != nullptr && j < index) {
    p = p->next;
    j++;
  }
  return p;
}

/**
 * @brief 按值查找节点
 */
template <IsLinkedNode NodePtr, typename ElemType>
  requires HasDataAndComparable<
      typename std::pointer_traits<NodePtr>::element_type, ElemType>
inline NodePtr LocateElem(NodePtr head, ElemType val) {
  if (head == nullptr)
    return nullptr;

  NodePtr p = head->next;
  while (p != nullptr && p->data != val) {
    p = p->next;
  }
  return p;
}

} // namespace ListUtils

#endif // LINKED_LIST_UTILS_HPP
