#ifndef INCLUDE_STACK_SHUNTINGYARD_HPP
#define INCLUDE_STACK_SHUNTINGYARD_HPP
#include <cstdlib>
#include <cstring>
#include <stack/SqStack.hpp>

#include "queue/SqQueue.hpp"

inline int _sym_code(char sym) {
  static int table[256] = {0};
  static int initialized = 0;
  if (!initialized) {
    table[' '] = -1;

    table['('] = 1;
    table[')'] = 2;

    table['+'] = 3;
    table['-'] = 3;

    table['*'] = 4;
    table['/'] = 4;
    table['%'] = 4;
  }
  return table[(unsigned char)sym];
}

// 建议使用指针来处理字符数组
// 返回值：一个动态分配的字符串，包含后缀表达式
inline char* infix_to_postfix(const char* infix_expr) {
  int len = 0;
  int valid_len = 0;
  for (int i = 0; infix_expr[i] != '\0'; i++) {
    char sym = infix_expr[i];
    int t_code = _sym_code(sym);
    len++;
    if (t_code == 0 || t_code == 3 || t_code == 4) {
      valid_len++;
    }
  }

  SqStack* sym_stack = NULL;
  bool init_sym_ok = InitStack(&sym_stack, sizeof(char), len);
  SqQueue* res_queue = NULL;
  bool init_res_ok = InitQueue(&res_queue, sizeof(char), valid_len + 1);
  if (!(init_res_ok && init_sym_ok)) {
    exit(1);
  }

  for (int i = 0; infix_expr[i] != '\0'; i++) {
    char sym = infix_expr[i];
    int code = _sym_code(sym);
    switch (code) {
      case -1:
        break;

      case 0: {
        EnQueue(res_queue, &sym);
        break;
      }

      case 1: {
        Push(sym_stack, &sym);
        break;
        ;
      }

      case 2: {
        char tmp = sym;
        while (GetTop(sym_stack, &tmp) && _sym_code(tmp) > 1) {
          Pop(sym_stack, &tmp);
          EnQueue(res_queue, &tmp);
        }
        Pop(sym_stack, &tmp);
        break;
      }

      case 3:
      case 4: {
        char top_sym;
        while (GetTop(sym_stack, &top_sym) &&
               _sym_code(top_sym) >= _sym_code(sym)) {
          Pop(sym_stack, &top_sym);
          EnQueue(res_queue, &top_sym);
        }
        Push(sym_stack, &sym);
        break;
      }
    }
  }

  char top_sym;
  while (Pop(sym_stack, &top_sym)) {
    if (_sym_code(top_sym) > 2) {
      EnQueue(res_queue, &top_sym);
    }
  }

  char* result = (char*)malloc(sizeof(char) * (valid_len + 1));
  int i = 0;
  while (!isSqQueueEmpty(res_queue)) {
    DeQueue(res_queue, &result[i]);  // 直接写入
    i++;
  }
  result[i] = '\0';  // 必须添加字符串结束符！
  DestroyStack(&sym_stack);
  DestorySqQueue(&res_queue);
  return result;
}

// 建议使用二级指针或栈结构来存储子表达式
// 每一个栈元素都是一个完整的字符串
inline void postfix_to_infix(const char* postfix_expr, char* result_buffer);

#endif  // !INCLUDE_STACK_SHUNTINGYARD_HPP
