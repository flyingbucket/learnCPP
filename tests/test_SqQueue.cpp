#include "catch2/catch.hpp"
#include "queue/SqQueue.h"

#define MaxSize 50

TEST_CASE("SqQueue: Initialization", "[SqQueue]") {
  SqQueue* q;
  bool initSuccess = InitQueue(&q, sizeof(int), 50);
  REQUIRE(initSuccess == true);

  REQUIRE(isSqQueueEmpty(q) == true);
  REQUIRE(isSqQueueFull(q) == false);
  REQUIRE(SqQueueLength(q) == 0);

  DestorySqQueue(&q);  // 记得清理内存
}

TEST_CASE("SqQueue: EnQueue and DeQueue basics", "[SqQueue]") {
  SqQueue* q;
  InitQueue(&q, sizeof(int), 50);

  SECTION("EnQueue a single element") {
    int val = 10;                     // 修改点：使用局部变量
    bool success = EnQueue(q, &val);  // 传递地址
    REQUIRE(success == true);
    REQUIRE(isSqQueueEmpty(q) == false);
    REQUIRE(SqQueueLength(q) == 1);
  }

  SECTION("EnQueue and DeQueue multiple elements") {
    int v1 = 10, v2 = 20, v3 = 30;  // 修改点：定义变量
    EnQueue(q, &v1);
    EnQueue(q, &v2);
    EnQueue(q, &v3);

    REQUIRE(SqQueueLength(q) == 3);

    int val = 0;
    REQUIRE(DeQueue(q, &val) == true);
    REQUIRE(val == 10);
    REQUIRE(SqQueueLength(q) == 2);

    REQUIRE(DeQueue(q, &val) == true);
    REQUIRE(val == 20);

    REQUIRE(DeQueue(q, &val) == true);
    REQUIRE(val == 30);

    REQUIRE(isSqQueueEmpty(q) == true);
    REQUIRE(SqQueueLength(q) == 0);

    REQUIRE(DeQueue(q, &val) == false);
  }
  DestorySqQueue(&q);
}

TEST_CASE("SqQueue: Full queue behavior", "[SqQueue]") {
  SqQueue* q;
  InitQueue(&q, sizeof(int), 50);

  SECTION("Fill the queue to its capacity") {
    int capacity = MaxSize - 1;
    for (int i = 0; i < capacity; ++i) {
      int val = i;  // 修改点：存储循环变量值
      REQUIRE(EnQueue(q, &val) == true);
    }

    REQUIRE(isSqQueueFull(q) == true);
    REQUIRE(SqQueueLength(q) == capacity);

    int overflowVal = 999;
    REQUIRE(EnQueue(q, &overflowVal) == false);
  }
  DestorySqQueue(&q);
}

TEST_CASE("SqQueue: Circular Wrap-around behavior", "[SqQueue]") {
  SqQueue* q;
  InitQueue(&q, sizeof(int), 50);

  for (int i = 0; i < 20; ++i) {
    int val = i;
    EnQueue(q, &val);
  }

  int val = 0;
  for (int i = 0; i < 10; ++i) {
    DeQueue(q, &val);
  }

  REQUIRE(SqQueueLength(q) == 10);

  for (int i = 20; i < 59; ++i) {
    int v = i;
    REQUIRE(EnQueue(q, &v) == true);
  }

  REQUIRE(isSqQueueFull(q) == true);
  REQUIRE(SqQueueLength(q) == MaxSize - 1);

  for (int i = 10; i < 59; ++i) {
    REQUIRE(DeQueue(q, &val) == true);
    REQUIRE(val == i);
  }

  REQUIRE(isSqQueueEmpty(q) == true);
  DestorySqQueue(&q);
}

TEST_CASE("SqQueue: Dynamic Capacity and Stress Test", "[SqQueue]") {
  SqQueue* q = nullptr;

  SECTION("Custom small capacity (Size 5)") {
    REQUIRE(InitQueue(&q, sizeof(int), 5) == true);  // 修改点：修复参数传递顺序

    for (int i = 0; i < 4; ++i) {
      int val = i * 10;
      REQUIRE(EnQueue(q, &val) == true);
    }

    REQUIRE(isSqQueueFull(q) == true);
    REQUIRE(SqQueueLength(q) == 4);
    int invalidVal = 99;
    REQUIRE(EnQueue(q, &invalidVal) == false);

    DestorySqQueue(&q);
  }

  SECTION("Wrap-around with custom capacity") {
    int cap = 10;
    InitQueue(&q, sizeof(int), cap);

    for (int i = 0; i < 8; ++i) {
      int v = i;
      EnQueue(q, &v);
    }

    int val;
    for (int i = 0; i < 5; ++i) DeQueue(q, &val);
    REQUIRE(SqQueueLength(q) == 3);

    for (int i = 0; i < 6; ++i) {
      int v = i + 100;
      REQUIRE(EnQueue(q, &v) == true);
    }

    REQUIRE(isSqQueueFull(q) == true);
    REQUIRE(SqQueueLength(q) == 9);

    DestorySqQueue(&q);
  }

  SECTION("Large capacity test") {
    int largeCap = 10000;
    REQUIRE(InitQueue(&q, sizeof(int), largeCap) == true);
    REQUIRE(isSqQueueEmpty(q) == true);

    int val = 1234;
    EnQueue(q, &val);
    REQUIRE(SqQueueLength(q) == 1);

    DestorySqQueue(&q);
  }
}
