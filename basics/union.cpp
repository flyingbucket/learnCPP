/// Conceptually, a union can be viewed as a structured, type-constrained
/// alternative to a raw `void*` pointer.
///
/// A union allocates a single contiguous block of memory large enough to hold
/// its largest variant, allowing that same memory region to be interpreted in
/// several predefined ways.
///
/// Note on C++ Standard compliance:
/// Strictly speaking, reading a union member other than the one most recently
/// written to is Undefined Behavior (UB) under strict aliasing rules.
///
/// Practical/Engineering Usage:
/// In performance-critical domains (e.g., graphics, physics, game engines),
/// developers often define unions whose variants have identical memory sizes
/// and layout-compatible representations. This technique allows for zero-cost
/// multi-view access to the same underlying data without falling back on unsafe
/// `void*` casts.

#include <cstdint>
#include <iostream>

// 示例 1: 基础用法 - 共享内存 space 节省资源
union BasicUnion {
  int32_t i;
  float f;
  char c;
};

// 示例 2: 常见工程实践 - 等大变体，实现多视图数据访问 (Type Punning / Alias
// Access)
union Color {
  // 视图 1: rgba标准的 32 位无符号整数, 从高位到低位按r g b a解读
  uint32_t rgba;

  // 视图 2: 结构体形式按通道独立访问 (与 rgba 在内存布局上保持一致)
  struct {
    uint8_t a;
    uint8_t b;
    uint8_t g;
    uint8_t r;
  } channels;

  // 视图 3: 数组形式按索引访问 (数组循环默认从低位向高位遍历，即按a g b r顺序)
  uint8_t raw[4];
};

int main() {
  // --- BasicUnion ---
  BasicUnion b;
  b.i = 42;
  std::cout << "BasicUnion int: " << b.i
            << std::endl; // 合法读取：最后写入的是 i

  // --- Color 多视图访问 ---
  Color color;
  // 使用 16 进制给整型赋值：0xAABBGGRR (小端序架构下：a=0xAA, b=0xBB, g=0xGG,
  // r=0xRR)
  color.rgba = 0xFF0088FF;

  std::cout << "\n--- Color Multi-view Access ---" << std::endl;
  // 视图 2 访问：按独立通道读取
  std::cout << "Red   channel (via struct): " << (int)color.channels.r
            << std::endl;
  std::cout << "Green   channel (via struct): " << (int)color.channels.g
            << std::endl;
  std::cout << "Blue   channel (via struct): " << (int)color.channels.b
            << std::endl;
  std::cout << "Alpha channel (via struct): " << (int)color.channels.a
            << std::endl;

  // 视图 3 访问：通过 raw 数组循环读取
  std::cout << "Raw bytes (via array): ";
  for (int i = 0; i < 4; ++i) {
    std::cout << "0x" << std::hex << (int)color.raw[i] << " ";
  }
  std::cout << std::endl;

  return 0;
}
