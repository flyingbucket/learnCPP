#ifndef INCLUDE_STACK_SHUNTINGYARD_HPP
#define INCLUDE_STACK_SHUNTINGYARD_HPP
#include <cctype>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "SqStack.hpp"
#include "queue/SqQueue.hpp"

inline int sym_code_(char sym) {
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
    int t_code = sym_code_(sym);
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
    int code = sym_code_(sym);
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
        while (GetTop(sym_stack, &tmp) && sym_code_(tmp) > 1) {
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
               sym_code_(top_sym) >= sym_code_(sym)) {
          Pop(sym_stack, &top_sym);
          EnQueue(res_queue, &top_sym);
        }
        Push(sym_stack, &sym);
        break;
      }
      default: {
      }
    }
  }

  char top_sym;
  while (Pop(sym_stack, &top_sym)) {
    if (sym_code_(top_sym) > 2) {
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

// 获取运算符优先级
// 99: 操作数 (最高)
// 3: *, /, %
// 1: +, -
// 0: 未知/无效
inline int get_prec_(char sym) {
  if (isalnum(sym)) return 99;  // 操作数优先级最高

  static int table[256] = {0};
  static int initialized = 0;
  if (!initialized) {
    table['+'] = 1;
    table['-'] = 1;
    table['*'] = 3;
    table['/'] = 3;
    table['%'] = 3;
    initialized = 1;
  }
  return table[(unsigned char)sym];
}

// 检查是否为非结合性运算符 (右侧同级需要加括号)
inline int is_non_associative_(char sym) {
  return (sym == '-' || sym == '/' || sym == '%');
}

// Pass 1 用的虚拟节点（只存数值，不存字符串）
typedef struct {
  size_t len;  // 字符串长度 (不含 \0)
  int prec;    // 优先级
} VirtualNode;

// Pass 2 用的真实节点
typedef struct {
  char* expr;  // 实际字符串
  int prec;    // 优先级
} RealNode;

// --- 核心函数 ---

// 辅助：计算逻辑（Pass 1）
inline size_t calculate_len_(const char* postfix) {
  size_t n = strlen(postfix);
  VirtualNode* stack = (VirtualNode*)malloc(sizeof(VirtualNode) * n);
  int top = -1;

  for (int i = 0; postfix[i] != '\0'; i++) {
    char sym = postfix[i];

    if (isspace(sym)) continue;

    if (isalnum(sym)) {
      // 操作数：长度为1，优先级99
      top++;
      stack[top].len = 1;
      stack[top].prec = 99;
    } else {
      // 操作符
      if (top < 1) {
        free(stack);
        return 0;
      }  // 错误：栈中元素不足

      VirtualNode r = stack[top--];
      VirtualNode l = stack[top--];

      int curr_prec = get_prec_(sym);

      // 判断括号
      int l_parens = (l.prec < curr_prec);
      int r_parens = (r.prec < curr_prec) ||
                     (r.prec == curr_prec && is_non_associative_(sym));

      // 计算新长度: (左) + 1(符号) + (右) + 括号
      size_t new_len =
          l.len + 1 + r.len + static_cast<size_t>(2 * (l_parens + r_parens));

      // 入栈
      top++;
      stack[top].len = new_len;
      stack[top].prec = curr_prec;
    }
  }

  size_t result = 0;
  if (top == 0) {
    result = stack[0].len;
  }

  free(stack);
  return result + 1;  // \0
}

// 辅助：构建逻辑（Pass 2）
inline size_t build_string_(const char* postfix, char* result_buff) {
  size_t n = strlen(postfix);
  RealNode* stack = (RealNode*)malloc(sizeof(RealNode) * n);
  int top = -1;

  for (int i = 0; postfix[i] != '\0'; i++) {
    char sym = postfix[i];

    if (isspace(sym)) continue;

    if (isalnum(sym)) {
      // 操作数
      top++;
      stack[top].expr = (char*)malloc(2);  // 1 char + \0
      stack[top].expr[0] = sym;
      stack[top].expr[1] = '\0';
      stack[top].prec = 99;
    } else {
      // 操作符
      if (top < 1) {
        while (top >= 0) free(stack[top--].expr);
        free(stack);
        return 0;
      }  // 错误清理

      RealNode r = stack[top--];
      RealNode l = stack[top--];

      int curr_prec = get_prec_(sym);

      int l_parens = (l.prec < curr_prec);
      int r_parens = (r.prec < curr_prec) ||
                     (r.prec == curr_prec && is_non_associative_(sym));

      // 计算所需内存
      size_t needed_size = strlen(l.expr) + strlen(r.expr) + 2 +
                           (l_parens ? 2 : 0) + (r_parens ? 2 : 0);

      char* new_str = (char*)malloc(needed_size);

      // 格式化字符串: (L) OP (R)
      char* ptr = new_str;

      // 左部分
      if (l_parens) *ptr++ = '(';
      strcpy(ptr, l.expr);
      ptr += strlen(l.expr);
      if (l_parens) *ptr++ = ')';

      // 中间
      *ptr++ = sym;

      // 右部分
      if (r_parens) *ptr++ = '(';
      strcpy(ptr, r.expr);
      ptr += strlen(r.expr);
      if (r_parens) *ptr++ = ')';

      *ptr = '\0';

      // 释放旧内存
      free(l.expr);
      free(r.expr);

      // 入栈
      top++;
      stack[top].expr = new_str;
      stack[top].prec = curr_prec;
    }
  }

  size_t total_written = 0;
  if (top == 0) {
    strcpy(result_buff, stack[0].expr);
    total_written = strlen(stack[0].expr);
    free(stack[0].expr);
  } else {
    // 异常情况：栈没空或者为空，清理内存
    while (top >= 0) free(stack[top--].expr);
  }

  free(stack);
  return total_written + 1;
}

inline size_t postfix_to_infix(const char* postfix_expr, char* result_buff) {
  if (postfix_expr == NULL) return 0;

  if (result_buff == NULL) {
    return calculate_len_(postfix_expr);
  }

  return build_string_(postfix_expr, result_buff);
}

#endif  // !INCLUDE_STACK_SHUNTINGYARD_HPP
