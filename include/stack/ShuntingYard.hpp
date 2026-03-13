#ifndef INCLUDE_STACK_SHUNTINGYARD_HPP
#define INCLUDE_STACK_SHUNTINGYARD_HPP
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "queue/SqQueue.hpp"
#include "stack/SqStack.hpp"

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

inline size_t infix_to_postfix(const char* infix_expr, char* result_buffer) {
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
  valid_len++;  // add place for '\0'

  if (result_buffer == NULL) {
    return valid_len;
  }

  SqStack* sym_stack = NULL;
  bool init_sym_ok = InitStack(&sym_stack, sizeof(char), len);
  SqQueue* res_queue = NULL;
  bool init_res_ok = InitQueue(&res_queue, sizeof(char), valid_len);
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

  // char* result = (char*)malloc(sizeof(char) * (valid_len + 1));
  int i = 0;
  while (!isSqQueueEmpty(res_queue)) {
    DeQueue(res_queue, &result_buffer[i]);  // 直接写入
    i++;
  }
  result_buffer[i] = '\0';  // 必须添加字符串结束符！
  DestroyStack(&sym_stack);
  DestorySqQueue(&res_queue);
  return i + 1;
}

inline size_t postfix_to_infix(const char* postfix_expr, char* result_buffer) {
  int len = 0;
  int branc_pair = 0;
  for (int i = 0; postfix_expr[i] != '\0'; i++) len++;
  SqStack* stack = NULL;
  bool init_stack_ok = InitStack(&stack, sizeof(char), len);
  if (!init_stack_ok) exit(1);
  for (int i = 0; postfix_expr[i] != '\0'; i++) {
    len++;
    char sym = postfix_expr[i];
    int code = _sym_code(sym);
    char top_sym;
    bool non_empty = GetTop(stack, &top_sym);
    if (code == 4 && non_empty && _sym_code(top_sym) == 3) {
      branc_pair += 1;
    }
    if (code >= 3) Push(stack, &sym);
  }

  int valid_len = len + branc_pair * 2 + 1;
  if (result_buffer == NULL) return valid_len;

  while (!isStackEmpty(stack)) {
    char tmp;
    Pop(stack, &tmp);
  }

  SqQueue* res_queue = NULL;
  bool init_res_queue_ok = InitQueue(&res_queue, sizeof(char), valid_len);
  SqQueue* tmp_queue = NULL;
  bool init_tmp_queue_ok = InitQueue(&tmp_queue, sizeof(char), valid_len);
  if (!init_res_queue_ok || !init_tmp_queue_ok) exit(1);

  for (int i = 0; postfix_expr[i] != '\0'; i++) {
    char sym = postfix_expr[i];
    int code = _sym_code(sym);
    switch (code) {
      case -1:
      case 1:
      case 2:
        break;

      case 0: {
        EnQueue(tmp_queue, &sym);
        break;
      }

      case 3: {
        Push(stack, &sym);
        break;
      }
      case 4: {
        char top;
        bool stack_not_empty = GetTop(stack, &top);
      }
    }
  }
}

#endif  // !INCLUDE_STACK_SHUNTINGYARD_HPP
