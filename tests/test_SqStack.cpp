#include <catch2/catch.hpp>

#include "stack/SqStack.hpp"

TEST_CASE("SqStack: Initialization and Destruction", "[SqStack]") {
  SqStack* stack = nullptr;

  SECTION("Default capacity initialization") {
    REQUIRE(InitStack(&stack) == true);
    REQUIRE(stack != nullptr);
    REQUIRE(stack->capacity == 500);  // 默认参数
    REQUIRE(stack->top == -1);
    REQUIRE(isStackEmpty(stack) == true);

    REQUIRE(DestroyStack(&stack) == true);
  }

  SECTION("Custom capacity initialization") {
    REQUIRE(InitStack(&stack, 10) == true);
    REQUIRE(stack != nullptr);
    REQUIRE(stack->capacity == 10);

    REQUIRE(DestroyStack(&stack) == true);
  }
}

TEST_CASE("SqStack: Core Operations (Push, Pop, GetTop)", "[SqStack]") {
  SqStack* stack = nullptr;
  // 使用较小的容量以便于测试溢出情况
  REQUIRE(InitStack(&stack, 3) == true);

  SECTION("Push and Pop behavior (LIFO)") {
    REQUIRE(isStackEmpty(stack) == true);

    // 压栈
    REQUIRE(Push(stack, 10) == true);
    REQUIRE(isStackEmpty(stack) == false);
    REQUIRE(Push(stack, 20) == true);

    // 获取栈顶元素但不移除
    int val = 0;
    REQUIRE(GetTop(stack, &val) == true);
    REQUIRE(val == 20);
    REQUIRE(isStackEmpty(stack) == false);  // 栈依然非空

    // 出栈（应该符合后进先出）
    REQUIRE(Pop(stack, &val) == true);
    REQUIRE(val == 20);

    REQUIRE(Pop(stack, &val) == true);
    REQUIRE(val == 10);

    // 此时栈应该为空
    REQUIRE(isStackEmpty(stack) == true);

    // 尝试从空栈出栈
    REQUIRE(Pop(stack, &val) == false);
  }

  SECTION("Capacity limit / Stack Overflow") {
    REQUIRE(Push(stack, 100) == true);
    REQUIRE(Push(stack, 200) == true);
    REQUIRE(Push(stack, 300) == true);

    // 栈已满，继续压栈应该返回 false
    REQUIRE(Push(stack, 400) == false);

    // 检查栈顶依然是最后一次成功压入的元素
    int val = 0;
    REQUIRE(GetTop(stack, &val) == true);
    REQUIRE(val == 300);
  }

  SECTION("Null pointer safety") {
    int val = 0;
    // 测试传递空指针时各个函数的健壮性
    REQUIRE(isStackEmpty(nullptr) == true);
    REQUIRE(Push(nullptr, 10) == false);
    REQUIRE(Pop(nullptr, &val) == false);
    REQUIRE(GetTop(nullptr, &val) == false);

    SqStack* null_stack = nullptr;
    REQUIRE(DestroyStack(&null_stack) == false);
  }

  // 每个 SECTION 运行结束后，统一清理资源
  DestroyStack(&stack);
}
