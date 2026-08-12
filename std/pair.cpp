#include <iostream>
#include <string>
#include <tuple>
#include <utility>

/// @brief Introduction to std::pair in C++
///
/// std::pair is a standard template struct defined in <utility> that couples
/// two heterogeneous values (first and second) together. It is extensively
/// utilized in map/multimap iterators, algorithms returning dual values, and
/// coordinate-like representations.

/// @brief Demonstrates pair creation using explicit constructors,
/// std::make_pair, and CTAD (C++17).
void demoPairConstruction() {
  std::cout << "=== std::pair: Construction and CTAD ===\n";

  // 1. Explicit template arguments
  std::pair<int, std::string> p1(1, "Alpha");

  // 2. Using helper function std::make_pair
  auto p2 = std::make_pair(2, "Beta");

  // 3. Class Template Argument Deduction (CTAD in C++17) - No explicit template
  // types required
  std::pair p3{3, "Gamma"};

  std::cout << "p1: (" << p1.first << ", " << p1.second << ")\n";
  std::cout << "p2: (" << p2.first << ", " << p2.second << ")\n";
  std::cout << "p3: (" << p3.first << ", " << p3.second << ")\n\n";
}

/// @brief Demonstrates member variable access and std::get<I> tuple-style
/// access.
void demoPairElementAccess() {
  std::cout << "=== std::pair: Element Access ===\n";

  std::pair<std::string, double> item("Coffee", 4.50);

  // Direct member access
  std::cout << "Item: " << item.first << ", Price: $" << item.second << "\n";

  // Tuple-style access via std::get<Index>
  std::cout << "std::get<0>: " << std::get<0>(item)
            << ", std::get<1>: " << std::get<1>(item) << "\n\n";
}

/// @brief Demonstrates unpacking pairs via std::tie (C++11) and Structured
/// Bindings (C++17).
void demoPairUnpacking() {
  std::cout << "=== std::pair: Unpacking (LHS / RHS) ===\n";

  std::pair<int, std::string> data{101, "Server_01"};

  // Unpacking to pre-declared variables using std::tie (C++11)
  int id;
  std::string name;
  std::tie(id, name) = data;
  std::cout << "Unpacked via std::tie -> ID: " << id << ", Name: " << name
            << "\n";

  // Modern C++17 Structured Binding (RHS decomposition)
  auto [node_id, node_name] = data;
  std::cout << "Unpacked via Structured Binding -> ID: " << node_id
            << ", Name: " << node_name << "\n\n";
}

/// @brief Demonstrates lexicographical comparison operators on std::pair.
void demoPairComparisons() {
  std::cout << "=== std::pair: Comparison Operations ===\n";

  std::pair<int, int> point1{1, 5};
  std::pair<int, int> point2{1, 10};

  // Pairs are compared lexicographically: first element primary, second element
  // secondary
  if (point1 < point2) {
    std::cout << "point1 (1, 5) is less than point2 (1, 10)\n\n";
  }
}

int main() {
  demoPairConstruction();
  demoPairElementAccess();
  demoPairUnpacking();
  demoPairComparisons();
  return 0;
}
