#include <iostream>
#include <string>
#include <string_view>

/// @file demo_string.cpp
/// @brief Introduction to std::string in C++
///
/// std::string (std::basic_string<char>) represents a contiguous sequence of
/// characters. It manages memory automatically on the heap (with Small String
/// Optimization for short strings), providing safe bounds-checking, dynamic
/// resizing, rich searching, and slice operations compared to legacy
/// null-terminated C-style strings (char*).

/// @brief Demonstrates string construction, initialization variants, and
/// element access.
void demoStringConstructionAndAccess() {
  std::cout << "=== std::string: Construction and Access ===\n";

  // 1. Various constructors
  std::string s1 = "Hello, Modern C++!"; // From string literal
  std::string s2(5, 'A');                // Filled constructor: "AAAAA"
  std::string s3(s1, 0, 5);              // Substring constructor: "Hello"

  // 2. Element Access
  std::cout << "First char (operator[]): " << s1[0] << "\n";
  std::cout << "Char at index 1 (at()): " << s1.at(1)
            << "\n"; // Throws std::out_of_range if invalid
  std::cout << "Front char: " << s1.front() << ", Back char: " << s1.back()
            << "\n";
  std::cout << "C-style string view: " << s1.c_str() << "\n\n";
}

/// @brief Demonstrates string modification, concatenation, insertion, and
/// substring extraction.
void demoStringModification() {
  std::cout << "=== std::string: Modifications ===\n";

  std::string str = "C++";

  // Append and concatenation
  str += " Programming";
  str.append(" Language");
  std::cout << "After append: " << str << "\n";

  // Insert and Erase
  str.insert(3, " 20"); // Insert at index 3
  std::cout << "After insert: " << str << "\n";

  str.erase(3, 3); // Erase 3 characters starting from index 3
  std::cout << "After erase: " << str << "\n";

  // Substring extraction (index, length)
  std::string sub = str.substr(0, 3);
  std::cout << "Extracted substring: " << sub << "\n\n";
}

/// @brief Demonstrates string searching, character finding, and comparison.
void demoStringSearchAndCompare() {
  std::cout << "=== std::string: Searching and Comparison ===\n";

  std::string text = "The quick brown fox jumps over the lazy dog";

  // Search substring
  size_t pos = text.find("fox");
  if (pos != std::string::npos) {
    std::cout << "'fox' found at position: " << pos << "\n";
  }

  // Find first of specific characters
  size_t vowel_pos = text.find_first_of("aeiou");
  std::cout << "First vowel found at index: " << vowel_pos << "\n";

  // Comparison
  std::string a = "Apple", b = "Banana";
  if (a < b) {
    std::cout << "'" << a << "' comes before '" << b
              << "' lexicographically.\n\n";
  }
}

/// @brief Demonstrates type conversions and non-owning std::string_view
/// interaction (C++17).
void demoStringConversionsAndStringView() {
  std::cout << "=== std::string: Conversions and std::string_view ===\n";

  // Numerical conversions
  int num = 2026;
  std::string num_str = std::to_string(num);
  int parsed_num = std::stoi("100");
  std::cout << "String to int: " << parsed_num << ", Int to string: " << num_str
            << "\n";

  // Interoperability with std::string_view (C++17 zero-cost string reference)
  std::string full_str = "High-Performance C++";
  std::string_view sv(full_str); // Non-owning reference
  std::cout << "string_view output: " << sv.substr(0, 16) << "\n\n";
}

int main() {
  demoStringConstructionAndAccess();
  demoStringModification();
  demoStringSearchAndCompare();
  demoStringConversionsAndStringView();
  return 0;
}
