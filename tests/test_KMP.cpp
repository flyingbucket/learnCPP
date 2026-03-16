#include <catch2/catch.hpp>
#include <cstring>

#include "string/KMP.hpp"
#include "string/basicOps.hpp"

TEST_CASE("get_next computes the correct next array", "[get_next]") {
  SECTION("No repeating patterns") {
    String* P = makeString("ABC");
    int next[3];
    get_next(P, next);
    REQUIRE(next[0] == -1);
    REQUIRE(next[1] == 0);
    REQUIRE(next[2] == 0);
  }

  SECTION("Repeating characters") {
    String* P = makeString("AAAA");
    int next[4];
    get_next(P, next);
    REQUIRE(next[0] == -1);
    REQUIRE(next[1] == 0);
    REQUIRE(next[2] == 1);
    REQUIRE(next[3] == 2);
  }

  SECTION("Mixed repeating patterns") {
    String* P = makeString("ABABA");
    int next[5];
    get_next(P, next);
    REQUIRE(next[0] == -1);
    REQUIRE(next[1] == 0);
    REQUIRE(next[2] == 0);
    REQUIRE(next[3] == 1);
    REQUIRE(next[4] == 2);
  }
}

TEST_CASE("KMP algorithm matches strings correctly", "[KMP]") {
  SECTION("Normal matches") {
    String* S = makeString("hello world");

    String* P1 = makeString("world");
    REQUIRE(KMP(S, P1) == 6);  // 匹配在末尾

    String* P2 = makeString("hello");
    REQUIRE(KMP(S, P2) == 0);  // 匹配在开头

    String* P3 = makeString("lo ");
    REQUIRE(3 == KMP(S, P3));  // 匹配在中间
  }

  SECTION("Multiple occurrences (returns first match)") {
    String* S = makeString("ababa");
    String* P = makeString("aba");
    REQUIRE(0 == KMP(S, P));
  }

  SECTION("No matches") {
    String* S = makeString("abcdef");
    String* P1 = makeString("xyz");
    REQUIRE(KMP(S, P1) == -1);  // 完全无关的字符串

    String* P2 = makeString("abf");
    REQUIRE(KMP(S, P2) == -1);  // 部分匹配但不完全
  }

  SECTION("Edge cases and boundary conditions") {
    String* S = makeString("abc");
    String* P_too_long = makeString("abcd");
    REQUIRE(KMP(S, P_too_long) == -1);  // 模式串比主串长

    String* P_empty = makeString("");
    REQUIRE(KMP(S, P_empty) == -1);  // 模式串为空

    REQUIRE(KMP(NULL, S) == -1);  // 空指针保护
    REQUIRE(KMP(S, NULL) == -1);
  }
}
