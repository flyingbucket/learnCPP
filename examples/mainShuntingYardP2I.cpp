#include <iostream>
#include <stack>
#include <string>

// 判断是否为操作符
bool isOperator(char c) { return c == '+' || c == '-' || c == '*' || c == '/'; }

std::string postfixToInfix(const std::string& postfix) {
  std::stack<std::string> s;

  for (char c : postfix) {
    // 忽略空格
    if (c == ' ') continue;

    if (!isOperator(c)) {
      // 1. 如果是操作数，直接入栈
      std::string operand(1, c);
      s.push(operand);
    } else {
      // 2. 如果是操作符，弹出两个操作数
      // 注意：如果栈内元素不足2个，说明表达式非法
      if (s.size() < 2) return "Error: Invalid Expression";

      std::string op2 = s.top();
      s.pop();  // 右操作数
      std::string op1 = s.top();
      s.pop();  // 左操作数

      // 3. 组合成中缀形式并压回栈
      // 简单的全括号策略
      std::string temp = "(" + op1 + c + op2 + ")";
      s.push(temp);
    }
  }

  if (s.empty()) return "";
  return s.top();
}

int main() {
  std::string postfix = "ab+c*";
  std::cout << "Postfix: " << postfix << "\n";
  std::cout << "Infix: " << postfixToInfix(postfix) << std::endl;
  return 0;
}
