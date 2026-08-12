#include <bitset>
#include <iostream>
#include <string>

/// @brief Introduction to std::bitset in C++
///
/// std::bitset<N> is a fixed-size container defined in <bitset> that stores N
/// bits. It combines space efficiency (similar to bit fields) with rich bitwise
/// manipulation API, compile-time bounds, and seamless conversions to/from
/// integer and binary strings.

/// @brief Demonstrates bitset initialization from unsigned integers and binary
/// strings.
void demoBitsetConstruction() {
  std::cout << "=== std::bitset: Construction ===\n";

  // Default construction (all bits initialized to 0)
  std::bitset<8> b1; // 00000000

  // From integer literal
  std::bitset<8> b2(42); // 42 in binary: 00101010

  // From string representation
  std::bitset<8> b3(std::string("11001010"));

  std::cout << "b1 (default): " << b1 << "\n";
  std::cout << "b2 (from 42) : " << b2 << "\n";
  std::cout << "b3 (string)  : " << b3 << "\n\n";
}

/// @brief Demonstrates bit modification operations (set, reset, flip,
/// operator[]).
void demoBitsetBitManipulation() {
  std::cout << "=== std::bitset: Bit Manipulations ===\n";

  std::bitset<8> bits; // 00000000

  // Set specific bit to 1
  bits.set(0);       // 00000001
  bits.set(3, true); // 00001001

  // Reset bit to 0
  bits.reset(0); // 00001000

  // Flip bits
  bits.flip(1); // 00001010
  bits.flip();  // Inverts all bits: 11110101

  // Array-style access
  bits[7] = 0;

  std::cout << "Manipulated bits: " << bits << "\n";
  std::cout << "Is bit 2 set? " << bits.test(2)
            << "\n\n"; // Safe test with bounds checking
}

/// @brief Demonstrates bitwise logical operators and shift operations.
void demoBitwiseOperators() {
  std::cout << "=== std::bitset: Bitwise Logic and Shifts ===\n";

  std::bitset<4> maskA("1100");
  std::bitset<4> maskB("1010");

  std::cout << "maskA: " << maskA << "\n";
  std::cout << "maskB: " << maskB << "\n";

  std::cout << "AND (&) : " << (maskA & maskB) << "\n";
  std::cout << "OR  (|) : " << (maskA | maskB) << "\n";
  std::cout << "XOR (^) : " << (maskA ^ maskB) << "\n";
  std::cout << "NOT (~) : " << (~maskA) << "\n";

  // Bit shifts
  std::cout << "Shift Left (<< 1) : " << (maskA << 1) << "\n";
  std::cout << "Shift Right (>> 1): " << (maskA >> 1) << "\n\n";
}

/// @brief Demonstrates bit inspection utility methods and string/number
/// conversions.
void demoBitsetUtilitiesAndConversions() {
  std::cout << "=== std::bitset: Utilities & Conversions ===\n";

  std::bitset<8> bits("00111100");

  std::cout << "bits: " << bits << "\n";

  std::cout << "Total bits count  : " << bits.size() << "\n";
  std::cout << "Number of set bits : " << bits.count() << "\n";

  // Status queries
  std::cout << "Are all bits set?  " << bits.all() << "\n";
  std::cout << "Is any bit set?    " << bits.any() << "\n";
  std::cout << "Are no bits set?   " << bits.none() << "\n";

  // Conversions
  unsigned long ulong_val = bits.to_ulong();
  std::string str_val = bits.to_string();
  std::cout << "As unsigned long: " << ulong_val << "\n";
  std::cout << "As std::string  : " << str_val << "\n\n";
}

int main() {
  demoBitsetConstruction();
  demoBitsetBitManipulation();
  demoBitwiseOperators();
  demoBitsetUtilitiesAndConversions();
  return 0;
}
