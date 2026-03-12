#include <catch2/catch.hpp>

#include "stack/LiStack.hpp"
TEST_CASE("LiStack (Headless Linked-List Stack) Operations", "[stack][list]") {
  LiStack stack = NULL;

  SECTION("1. Initial state should be empty") {
    REQUIRE(isStackEmpty(stack) == true);

    ElemType val;
    // 空栈获取栈顶或弹出应该失败
    REQUIRE(GetTop(stack, &val) == false);
    REQUIRE(Pop(&stack, &val) == false);
  }

  SECTION("2. Push and GetTop should work correctly") {
    REQUIRE(Push(&stack, 10) == true);
    REQUIRE(isStackEmpty(stack) == false);

    ElemType topVal = 0;
    REQUIRE(GetTop(stack, &topVal) == true);
    REQUIRE(topVal == 10);

    // 再次压栈
    REQUIRE(Push(&stack, 20) == true);
    REQUIRE(GetTop(stack, &topVal) == true);
    REQUIRE(topVal == 20);

    // 测试结束后清理内存
    DestroyStack(&stack);
  }

  SECTION("3. Pop elements in LIFO order") {
    Push(&stack, 1);
    Push(&stack, 2);
    Push(&stack, 3);

    ElemType val = 0;

    REQUIRE(Pop(&stack, &val) == true);
    REQUIRE(val == 3);

    REQUIRE(Pop(&stack, &val) == true);
    REQUIRE(val == 2);

    REQUIRE(Pop(&stack, &val) == true);
    REQUIRE(val == 1);

    // 栈应该再次变为空
    REQUIRE(isStackEmpty(stack) == true);
    REQUIRE(Pop(&stack, &val) == false);
  }

  SECTION("4. DestroyStack should free memory and reset pointer") {
    Push(&stack, 100);
    Push(&stack, 200);
    Push(&stack, 300);

    REQUIRE(DestroyStack(&stack) == true);

    // DestroyStack 内部的 while 循环结束时，*s 会被置为 NULL
    REQUIRE(stack == NULL);
    REQUIRE(isStackEmpty(stack) == true);

    // 尝试销毁空栈应该返回 false
    REQUIRE(DestroyStack(&stack) == false);
  }
}
