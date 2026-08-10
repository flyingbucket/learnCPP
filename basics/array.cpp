#include <initializer_list>
#include <iostream>

// 通用模板函数：通过引用接收原生数组，防止指针退化，并自动推导数组类型 T
// 和长度N
template <typename T, size_t N> void printArray(const T (&arr)[N]) {
  std::cout << "[ ";
  for (size_t i = 0; i < N; ++i) {
    std::cout << arr[i] << (i + 1 < N ? ", " : " ");
  }
  std::cout << "]\n";
}

int main() {
  std::cout << "1. 显式指定大小与初始化列表:\n";
  int arr1[5] = {10, 20, 30, 40, 50};
  printArray(arr1);
  std::cout << "\n";

  std::cout << "2. 自动推导数组大小:\n";
  double arr2[] = {1.1, 2.2, 3.3};
  printArray(arr2);
  std::cout << "\n";

  std::cout << "3. 部分初始化:\n";
  int arr3[5] = {1, 2};
  printArray(arr3);
  std::cout << "\n";

  // C++11 列表初始化（可以省略 '='，且不允许窄化转换，例如填 3.14会编译报错）
  std::cout << "4. C++11 列表初始化:\n";
  int arr4[]{100, 200, 300};
  printArray(arr4);
  std::cout << "\n";

  std::cout << "5. auto 类型推导 (推导出 std::initializer_list<int>):\n";
  auto arr5 = {1, 2, 3};

  // std::initializer_list 不是原生数组，无法直接传给上面的 printArray
  // 模板，但可以用 range-based for 遍历
  std::cout << "[ ";
  for (int val : arr5) {
    std::cout << val << " ";
  }
  std::cout << "]\n";

  return 0;
}
