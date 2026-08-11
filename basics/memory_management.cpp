/// @brief A sample class to demonstrate constructor/destructor invocation.

#include <cstdlib> // For malloc, free
#include <iostream>
#include <string>
#include <vector>

class Widget {
public:
  int id;

  Widget(int i = 0) : id(i) {
    std::cout << "  [Widget] Constructor called for ID: " << id << "\n";
  }

  ~Widget() {
    std::cout << "  [Widget] Destructor called for ID: " << id << "\n";
  }
};

// @brief Demonstrates the use of malloc and free (C-style memory management)
void demo_malloc_free() {
  std::cout << "--- malloc / free Demo ---\n";

  // Allocate raw memory for a single Widget
  // WARNING: malloc DOES NOT call the constructor!
  std::cout << "Allocating memory with malloc...\n";
  Widget *w1 = static_cast<Widget *>(std::malloc(sizeof(Widget)));

  if (w1 != nullptr) {
    // We must manually initialize the data because the constructor was not
    // called
    w1->id = 100;
    std::cout << "  w1 ID set to: " << w1->id << "\n";

    // Free the memory
    // WARNING: free DOES NOT call the destructor!
    std::cout << "Freeing memory with free...\n";
    std::free(w1);
    w1 = nullptr;
  }
  std::cout << "\n";
}

//  @brief Demonstrates the use of new and delete (C++ style memory management)
void demo_new_delete() {
  std::cout << "--- new / delete Demo ---\n";

  // Allocate memory and initialize using new
  // The constructor IS called automatically.
  std::cout << "Creating object with new...\n";
  Widget *w2 = new Widget(200);

  // Free the memory using delete
  // The destructor IS called automatically.
  std::cout << "Destroying object with delete...\n";
  delete w2;
  w2 = nullptr;

  std::cout << "\nCreating array of objects with new[]...\n";
  // Allocate an array of Widgets
  // The default constructor is called 3 times.
  Widget *w_array = new Widget[3]{0, 1, 2};

  std::cout << "Destroying array with delete[]...\n";
  // Must use delete[] for arrays to ensure all destructors are called.
  delete[] w_array;
  w_array = nullptr;

  std::cout << "\n";
}

// @brief Demonstrates the use of the 'auto' keyword (C++11 type inference)
void demo_auto() {
  std::cout << "--- auto Keyword Demo ---\n";

  // Basic type inference
  auto a = 42;         // Deduced as int
  auto b = 3.14159;    // Deduced as double
  auto msg = "Hello!"; // Deduced as const char*

  std::cout << "  Auto deduced types successfully. a: " << a << ", b: " << b
            << ", msg:" << msg << "\n";

  // Type inference with new
  auto *w3 = new Widget(300); // Deduced as Widget*
  delete w3;

  // Simplifying complex types (e.g., standard library iterators)
  std::vector<std::string> names = {"Alice", "Bob", "Charlie"};

  std::cout << "  Iterating using auto: ";
  // Instead of: std::vector<std::string>::const_iterator it = names.cbegin()
  for (auto it = names.cbegin(); it != names.cend(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << "\n\n";
}

/**
 * @brief Main function
 */
int main() {
  std::cout << "======================================\n";
  std::cout << " C++ Memory Management & Auto Demo \n";
  std::cout << "======================================\n\n";

  demo_malloc_free();
  demo_new_delete();
  demo_auto();

  return 0;
}
