#include <iostream>
#include <string>

struct Player {
  std::string name;
  int health;
};

int main() {
  std::cout
      << "===================================================================="
         "\n"
      << "[1. Semantic vs. Low-Level Concept]\n"
      << "At the semantic level, a reference is an alias for an existing "
         "variable.\n"
      << "Operating on a reference is equivalent to operating directly on the\n"
      << "object itself.\n\n"
      << "At the underlying assembly and memory level, a reference is "
         "typically\n"
      << "implemented as a thin pointer rather than a fat pointer (like "
         "Rust's\n"
      << "slice references). Implicit dereferencing is automatically handled "
         "by\n"
      << "the compiler.\n"
      << "===================================================================="
         "\n\n";

  // Re-binding Test: Pointers vs. References
  int x = 100;
  int y = 200;
  std::cout << "Initial values -> x: " << x << ", y: " << y << "\n\n";

  std::cout << "--- Pointer: Can change target address ---\n";
  int *p = &x;
  std::cout << "p points to x, value: " << *p << "\n";
  p = &y;
  std::cout << "p re-pointed to y, value: " << *p << "\n\n";

  std::cout << "--- Reference: Cannot change target binding ---\n";
  int &r = x;
  std::cout << "r references x, value: " << r << "\n";
  std::cout << "Re-assigning 'r = y' assigns y's value to x rather than "
               "re-binding r:\n";
  r = y; // Equivalent to x = y
  std::cout << "Resulting values -> x: " << x << ", y: " << y << "\n\n";

  // Member Access: Structs / Classes ('->' vs '.')
  std::cout << "--- Member Access: Pointer (->) vs Reference (.) ---\n";
  Player player{"Alice", 100};
  Player *ptr_player = &player;
  Player &ref_player = player;

  // Pointer uses '->' to access members
  ptr_player->health = 90;
  std::cout << "Updated via pointer (ptr_player->health): " << player.health
            << "\n";

  // Reference uses '.' directly, exactly like a normal object
  ref_player.health = 80;
  std::cout << "Updated via reference (ref_player.health): " << player.health
            << "\n\n";

  // Operator sizeof Behavior
  std::cout << "--- sizeof Operator Behavior ---\n";
  // sizeof(pointer) returns the size of the address
  std::cout << "sizeof(ptr_player): " << sizeof(ptr_player)
            << " bytes (pointer variable size)\n";

  // sizeof(reference) returns the exact size of the underlying target object
  std::cout << "sizeof(ref_player): " << sizeof(ref_player)
            << " bytes (referred object size)\n\n";

  // Native Arrays: Pointer Decay vs Reference Preservation
  std::cout << "--- Native Arrays: Pointer Decay vs Reference Bound ---\n";
  int arr[5] = {10, 20, 30, 40, 50};

  int *ptr_arr = arr;      // Array decays to a pointer to its first element
  int (&ref_arr)[5] = arr; // Reference binds to the ENTIRE array object

  std::cout << "sizeof(arr)      : " << sizeof(arr)
            << " bytes (5 * sizeof(int))\n";
  std::cout << "sizeof(ptr_arr)  : " << sizeof(ptr_arr)
            << " bytes (decayed to raw pointer)\n";
  std::cout << "sizeof(ref_arr)  : " << sizeof(ref_arr)
            << " bytes (retains full array type)\n\n";

  // Multilevel Indirection: Multi-level Pointers vs References
  std::cout << "--- Multilevel Indirection: Multi-level Pointers vs "
               "Single-level Alias ---\n";
  int val = 42;
  int *p1 = &val;
  int **p2 = &p1; // Multi-level pointer: Pointer to a pointer

  std::cout << "p2 points to p1, which points to val. Dereferencing **p2: "
            << **p2 << "\n";

  int &r1 = val;
  int &r2 = r1; // r2 is an alias of r1, which is directly bound to val.
                // There is NO "reference to reference" type syntax like int&&&
                // in C++.
  std::cout
      << "r2 is a reference to r1 (effectively referencing val directly): "
      << r2 << "\n";

  return 0;
}
