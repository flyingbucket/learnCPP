#include "catch2/catch.hpp"
#include "stack/ShuntingYard.hpp"

TEST_CASE("中缀转后缀：双调用模式验证", "[infix_to_postfix]") {
  // 辅助 Lambda：模拟双调用逻辑并返回结果字符串
  auto double_call_helper = [](const char* infix) -> std::string {
    // 1. 第一次调用：查询长度
    size_t required_len = infix_to_postfix(infix, NULL);

    // 2. 分配内存 (使用 unique_ptr 自动管理，防止测试失败导致内存泄漏)
    std::unique_ptr<char[]> buffer(new char[required_len]);

    // 3. 第二次调用：实际填充
    size_t written_len = infix_to_postfix(infix, buffer.get());

    // 验证两次返回的长度是否一致
    if (required_len != written_len) {
      return "ERROR: Length Mismatch";
    }

    return std::string(buffer.get());
  };

  SECTION("基础运算验证") {
    REQUIRE(double_call_helper("a+b") == "ab+");
    // 额外验证长度： "ab+" 是 3 字符 + 1 个 \0 = 4
    REQUIRE(infix_to_postfix("a+b", NULL) == 4);
  }

  SECTION("优先级处理验证") {
    REQUIRE(double_call_helper("a+b*c") == "abc*+");
    REQUIRE(infix_to_postfix("a+b*c", NULL) == 6);
  }

  SECTION("嵌套括号验证") {
    REQUIRE(double_call_helper("(a+b)*c") == "ab+c*");
    REQUIRE(double_call_helper("a*(b+c*d)") == "abcd*+*");
    // "abcd*+*" 是 7 字符 + 1 = 8
    REQUIRE(infix_to_postfix("a*(b+c*d)", NULL) == 8);
  }

  SECTION("空表达式或空格处理") {
    REQUIRE(double_call_helper("  ") == "");
    REQUIRE(infix_to_postfix("  ", NULL) == 1);  // 仅含 \0
  }
}

// TEST_CASE("后缀转中缀转换正确性验证", "[postfix_to_infix]") {
//   char buffer[256];  // 假设你有足够的缓冲区空间
//
//   // 简单验证
//   postfix_to_infix("ab+", buffer);
//   REQUIRE(std::string(buffer) == "(a+b)");
//
//   // 复杂验证
//   postfix_to_infix("abc*+", buffer);
//   REQUIRE(std::string(buffer) == "(a+(b*c))");
//
//   // 深度验证
//   postfix_to_infix("abcd*+*", buffer);
//   REQUIRE(std::string(buffer) == "(a*(b+(c*d)))");
// }
