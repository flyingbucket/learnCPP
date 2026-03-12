#include "catch2/catch.hpp"
#include "queue/SqQueue.hpp"

TEST_CASE("SqQueue: Initialization", "[SqQueue]") {
  SqQueue *q;
  // 根据签名，InitQueue 接收一个指针
  bool initSuccess = InitQueue(&q);
  REQUIRE(initSuccess == true);

  // 刚初始化好的队列应该为空，且长度为0，且不为满
  REQUIRE(isSqQueueEmpty(q) == true);
  REQUIRE(isSqQueueFull(q) == false);
  REQUIRE(SqQueueLength(q) == 0);
}

TEST_CASE("SqQueue: EnQueue and DeQueue basics", "[SqQueue]") {
  SqQueue *q;
  InitQueue(&q);

  SECTION("EnQueue a single element") {
    bool success = EnQueue(q, 10);
    REQUIRE(success == true);
    REQUIRE(isSqQueueEmpty(q) == false);
    REQUIRE(SqQueueLength(q) == 1);
  }

  SECTION("EnQueue and DeQueue multiple elements") {
    EnQueue(q, 10);
    EnQueue(q, 20);
    EnQueue(q, 30);

    REQUIRE(SqQueueLength(q) == 3);

    int val = 0;

    // 测试出队顺序 (FIFO)
    REQUIRE(DeQueue(q, &val) == true);
    REQUIRE(val == 10);
    REQUIRE(SqQueueLength(q) == 2);

    REQUIRE(DeQueue(q, &val) == true);
    REQUIRE(val == 20);

    REQUIRE(DeQueue(q, &val) == true);
    REQUIRE(val == 30);

    // 全部出队后应该为空
    REQUIRE(isSqQueueEmpty(q) == true);
    REQUIRE(SqQueueLength(q) == 0);

    // 空队列继续出队应该失败
    REQUIRE(DeQueue(q, &val) == false);
  }
}

TEST_CASE("SqQueue: Full queue behavior", "[SqQueue]") {
  SqQueue *q;
  InitQueue(&q);

  SECTION("Fill the queue to its capacity") {
    // 循环队列为了区分空和满，牺牲了一个节点
    // 所以实际最大容量应该是 MaxSize - 1
    int capacity = MaxSize - 1;

    for (int i = 0; i < capacity; ++i) {
      REQUIRE(EnQueue(q, i) == true);
    }

    // 达到容量上限后，状态应该为满
    REQUIRE(isSqQueueFull(q) == true);
    REQUIRE(SqQueueLength(q) == capacity);

    // 满队列继续入队应该失败
    REQUIRE(EnQueue(q, 999) == false);
  }
}

TEST_CASE("SqQueue: Circular Wrap-around behavior", "[SqQueue]") {
  SqQueue *q;
  InitQueue(&q);

  // 1. 先入队一部分数据 (例如 20 个)
  for (int i = 0; i < 20; ++i) {
    EnQueue(q, i);
  }

  // 2. 出队一部分数据，让 front 指针向后移动 (例如出队 10 个)
  int val = 0;
  for (int i = 0; i < 10; ++i) {
    DeQueue(q, &val);
  }

  // 此时队列里还剩下 10 个元素 (10~19)
  REQUIRE(SqQueueLength(q) == 10);

  // 3. 继续入队，触发 rear 指针的“折返”现象
  // 队列还能容纳 (MaxSize - 1) - 10 = 39 个元素
  for (int i = 20; i < 59; ++i) {
    REQUIRE(EnQueue(q, i) == true);
  }

  // 此时队列应该又满了，且 rear 指针肯定已经越过数组末尾回到了头部
  REQUIRE(isSqQueueFull(q) == true);
  REQUIRE(SqQueueLength(q) == MaxSize - 1);

  // 4. 验证循环出队的数据是否严格遵循 FIFO 且数据正确
  for (int i = 10; i < 59; ++i) {
    REQUIRE(DeQueue(q, &val) == true);
    REQUIRE(val == i);
  }

  // 最终应该再次变为空
  REQUIRE(isSqQueueEmpty(q) == true);
}
