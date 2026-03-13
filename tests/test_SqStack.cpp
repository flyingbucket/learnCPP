#include <catch2/catch.hpp>

#include "stack/SqStack.hpp"

// 用于测试泛型存储的自定义结构体
struct Player {
  int id;
  float health;
};

TEST_CASE("栈的初始化与销毁 (Init & Destroy)", "[SqStack]") {
  SqStack* s = nullptr;

  SECTION("正常初始化和销毁") {
    REQUIRE(InitStack(&s, sizeof(int), 10) == true);
    REQUIRE(s != nullptr);
    REQUIRE(s->capacity == 10);
    REQUIRE(s->elem_size == sizeof(int));
    REQUIRE(s->top == -1);
    REQUIRE(isStackEmpty(s) == true);

    REQUIRE(DestroyStack(&s) == true);
    REQUIRE(s == nullptr);  // 验证二级指针置空是否生效
  }

  SECTION("空指针安全测试") {
    REQUIRE(DestroyStack(nullptr) == false);
    REQUIRE(isStackEmpty(nullptr) == true);
  }
}

TEST_CASE("存储基本类型 (int)", "[SqStack][Basic]") {
  SqStack* s = nullptr;
  InitStack(&s, sizeof(int), 3);  // 容量为 3

  SECTION("正常入栈与出栈") {
    int val1 = 10, val2 = 20;
    REQUIRE(Push(s, &val1) == true);
    REQUIRE(Push(s, &val2) == true);
    REQUIRE(isStackEmpty(s) == false);

    int out_val = 0;
    REQUIRE(GetTop(s, &out_val) == true);
    REQUIRE(out_val == 20);  // GetTop 不应该改变栈顶

    REQUIRE(Pop(s, &out_val) == true);
    REQUIRE(out_val == 20);  // 第一次 Pop 应该是 20

    REQUIRE(Pop(s, &out_val) == true);
    REQUIRE(out_val == 10);  // 第二次 Pop 应该是 10

    REQUIRE(isStackEmpty(s) == true);
  }

  SECTION("栈满测试") {
    int val = 1;
    REQUIRE(Push(s, &val) == true);  // 1/3
    REQUIRE(Push(s, &val) == true);  // 2/3
    REQUIRE(Push(s, &val) == true);  // 3/3 (已满)

    int overflow_val = 99;
    REQUIRE(Push(s, &overflow_val) == false);  // 容量已满，应该返回 false
  }

  SECTION("栈空出栈测试") {
    int val = 0;
    REQUIRE(Pop(s, &val) == false);     // 空栈 Pop 应该失败
    REQUIRE(GetTop(s, &val) == false);  // 空栈 GetTop 应该失败
  }

  DestroyStack(&s);
}

TEST_CASE("存储复杂结构体 (Struct)", "[SqStack][Struct]") {
  SqStack* s = nullptr;
  InitStack(&s, sizeof(Player), 5);  // 存入自定义的 Player 结构体

  Player p1 = {1, 100.0f};
  Player p2 = {2, 85.5f};

  REQUIRE(Push(s, &p1) == true);
  REQUIRE(Push(s, &p2) == true);

  Player out_p;
  REQUIRE(Pop(s, &out_p) == true);
  REQUIRE(out_p.id == 2);
  REQUIRE(out_p.health == 85.5f);

  REQUIRE(Pop(s, &out_p) == true);
  REQUIRE(out_p.id == 1);
  REQUIRE(out_p.health == 100.0f);

  DestroyStack(&s);
}
