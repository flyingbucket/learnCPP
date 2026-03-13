#include <cstring>
#include <memory>
#include <string>

#include "catch2/catch.hpp"
#include "stack/ShuntingYard.hpp"  // 包含你的头文件

// --- 辅助函数：简化测试调用 ---

// 1. 测试 infix_to_postfix 的辅助函数
std::string test_in2post(const char* infix) {
  if (infix == nullptr) return "";

  // 第一次调用：获取长度
  size_t len = infix_to_postfix(infix, nullptr);
  if (len == 0) return "";

  // 分配内存
  std::unique_ptr<char[]> buffer(new char[len]);

  // 第二次调用：转换
  size_t written = infix_to_postfix(infix, buffer.get());

  // 验证长度一致性
  REQUIRE(len == written);

  return std::string(buffer.get());
}

// 2. 测试 postfix_to_infix 的辅助函数
std::string test_post2in(const char* postfix) {
  if (postfix == nullptr) return "";

  // 第一次调用：获取长度
  size_t len = postfix_to_infix(postfix, nullptr);
  if (len == 0) return "";

  // 分配内存
  std::unique_ptr<char[]> buffer(new char[len]);

  // 第二次调用：转换
  size_t written = postfix_to_infix(postfix, buffer.get());

  // 验证长度一致性（注意：如果你的实现中有异常分支导致提前返回0，这里会fail）
  // 对于有效输入，通常 len == written
  if (written == 0) return "";  // 转换失败

  return std::string(buffer.get());
}

// --- 测试套件 ---

TEST_CASE("Infix to Postfix Conversion", "[infix_to_postfix]") {
  SECTION("基础运算") {
    REQUIRE(test_in2post("a+b") == "ab+");
    REQUIRE(test_in2post("a-b") == "ab-");
    REQUIRE(test_in2post("a*b") == "ab*");
    REQUIRE(test_in2post("a/b") == "ab/");
  }

  SECTION("优先级处理") {
    // 乘除优先于加减
    REQUIRE(test_in2post("a+b*c") == "abc*+");
    REQUIRE(test_in2post("a*b+c") == "ab*c+");
    // 括号改变优先级
    REQUIRE(test_in2post("(a+b)*c") == "ab+c*");
    REQUIRE(test_in2post("a*(b+c)") == "abc+*");
  }

  SECTION("复杂嵌套") {
    REQUIRE(test_in2post("a*(b+c*d)+e") == "abcd*+*e+");
    REQUIRE(test_in2post("((a+b)*c)-d") == "ab+c*d-");
  }
}

TEST_CASE("Postfix to Infix Conversion (Minimal Parentheses)",
          "[postfix_to_infix]") {
  SECTION("基础运算还原") {
    REQUIRE(test_post2in("ab+") == "a+b");
    REQUIRE(test_post2in("ab-") == "a-b");
    REQUIRE(test_post2in("ab*") == "a*b");
  }

  SECTION("优先级还原 (无需括号)") {
    // ab*c+ -> a*b+c
    REQUIRE(test_post2in("ab*c+") == "a*b+c");
    // abc*+ -> a+b*c
    REQUIRE(test_post2in("abc*+") == "a+b*c");
  }

  SECTION("优先级还原 (需要括号)") {
    // ab+c* -> (a+b)*c (因为 + 低于 *)
    REQUIRE(test_post2in("ab+c*") == "(a+b)*c");

    // abc+* -> a*(b+c)
    REQUIRE(test_post2in("abc+*") == "a*(b+c)");
  }

  SECTION("结合性处理 (左结合 vs 右结合)") {
    // 1. 加法 (结合律)
    // ab+c+ -> a+b+c (左边不需要括号)
    REQUIRE(test_post2in("ab+c+") == "a+b+c");
    // abc++ -> a+(b+c) -> a+b+c (右边不需要括号，因为 + 是结合的)
    // 注意：根据你的逻辑 int r_parens = ... || (r.prec == curr_prec &&
    // _is_non_associative(sym)); 对于 '+'，_is_non_associative 返回
    // false，所以不会加括号。
    REQUIRE(test_post2in("abc++") == "a+b+c");

    // 2. 减法 (非结合律)
    // ab-c- -> a-b-c (左边不需要括号)
    REQUIRE(test_post2in("ab-c-") == "a-b-c");

    // abc-- -> a-(b-c) (右边必须加括号！)
    // 逻辑验证：curr='-', r='-', prec相等，且 '-' 是 non_associative -> 加括号
    REQUIRE(test_post2in("abc--") == "a-(b-c)");

    // 3. 除法 (非结合律)
    REQUIRE(test_post2in("ab/c/") == "a/b/c");
    REQUIRE(test_post2in("abc//") == "a/(b/c)");
  }

  SECTION("混合运算复杂案例") {
    // abc+- -> a-(b+c)
    // 右边是 +, 优先级1; 当前是 -, 优先级1;
    // 优先级相等，且 - 是 non_associative -> 右边加括号
    REQUIRE(test_post2in("abc+-") == "a-(b+c)");

    // ab-c+ -> a-b+c (加减同级，左结合，自然顺序)
    REQUIRE(test_post2in("ab-c+") == "a-b+c");

    // a/(b*c)
    // b*c (3), / (3). / 是 non_associative -> 右边加括号
    REQUIRE(test_post2in("abc*/") == "a/(b*c)");
  }
}

TEST_CASE("Round Trip Consistency (Infix -> Postfix -> Infix)",
          "[round_trip]") {
  // 验证：Infix -> Postfix -> Infix 应该逻辑等价
  // 注意：结果字符串可能不完全相同（因为去掉了多余括号），但数学语义应一致。
  // 如果原表达式本身就没有多余括号，那么应该字符串完全相等。

  auto round_trip = [](const char* original) -> std::string {
    std::string post = test_in2post(original);
    return test_post2in(post.c_str());
  };

  SECTION("Standard Cases") {
    // 简单的一致
    REQUIRE(round_trip("a+b") == "a+b");
    REQUIRE(round_trip("a*b+c") == "a*b+c");
    REQUIRE(round_trip("(a+b)*c") == "(a+b)*c");
  }

  SECTION("Simplification Cases") {
    // 原表达式有多余括号，转换回来后应该被优化掉
    // (a*b) -> ab* -> a*b
    REQUIRE(round_trip("(a*b)") == "a*b");

    // (a+b)+c -> ab+c+ -> a+b+c
    REQUIRE(round_trip("(a+b)+c") == "a+b+c");

    // a+(b+c) -> abc++ -> a+b+c
    REQUIRE(round_trip("a+(b+c)") == "a+b+c");
  }

  SECTION("Necessary Parentheses Retention") {
    // a-(b-c) -> abc-- -> a-(b-c) (括号必须保留)
    REQUIRE(round_trip("a-(b-c)") == "a-(b-c)");
  }
}
