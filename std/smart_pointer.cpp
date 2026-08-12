#include <iostream>
#include <memory>
#include <string>

/// @brief Introduction to C++ Smart Pointers (std::unique_ptr, std::shared_ptr,
/// std::weak_ptr)
///
/// Smart pointers defined in <memory> automate heap memory lifecycle management
/// using RAII. They prevent memory leaks, dangling pointers, and double-free
/// errors without GC overhead.
/// - std::unique_ptr: Exclusive, non-copyable ownership (zero-overhead).
/// - std::shared_ptr: Shared, reference-counted ownership.
/// - std::weak_ptr: Non-owning observer to break circular reference cycles.

struct Resource {
  std::string name;
  Resource(const std::string &n) : name(n) {
    std::cout << "  [Resource] " << name << " acquired.\n";
  }
  ~Resource() { std::cout << "  [Resource] " << name << " destroyed.\n"; }
  void doWork() const {
    std::cout << "  [Resource] " << name << " is processing task.\n";
  }
};

/// @brief Demonstrates std::unique_ptr for exclusive ownership and move-only
/// semantics.
void demoUniquePtr() {
  std::cout << "=== Smart Pointers: std::unique_ptr ===\n";

  // Preferred creation method using std::make_unique (C++14)
  std::unique_ptr<Resource> ptr1 = std::make_unique<Resource>("UniqueRes");
  ptr1->doWork();

  // std::unique_ptr cannot be copied!
  // std::unique_ptr<Resource> ptr2 = ptr1; // Compile Error!

  // Ownership transfer via Move Semantics
  std::unique_ptr<Resource> ptr2 = std::move(ptr1);
  if (!ptr1) {
    std::cout << "  ptr1 is now nullptr after move.\n";
  }
  ptr2->doWork();

  // Resource is automatically freed when ptr2 leaves scope
  std::cout << "\n";
}

/// @brief Demonstrates std::shared_ptr for reference-counted shared ownership.
void demoSharedPtr() {
  std::cout << "=== Smart Pointers: std::shared_ptr ===\n";

  // Create shared resource with std::make_shared (combines control block +
  // object allocation)
  std::shared_ptr<Resource> sp1 = std::make_shared<Resource>("SharedRes");
  std::cout << "  Initial use_count: " << sp1.use_count() << "\n";

  {
    // Copying increases reference count
    std::shared_ptr<Resource> sp2 = sp1;
    std::cout << "  use_count inside nested scope: " << sp1.use_count() << "\n";
    sp2->doWork();
  } // sp2 destroyed here, reference count decreases

  std::cout << "  use_count after scope exit: " << sp1.use_count() << "\n\n";
}

/// @brief Demonstrates std::weak_ptr as a non-owning observer to avoid circular
/// references.
void demoWeakPtr() {
  std::cout << "=== Smart Pointers: std::weak_ptr ===\n";

  std::weak_ptr<Resource> wp;

  {
    std::shared_ptr<Resource> sp = std::make_shared<Resource>("ObservedRes");
    wp = sp; // weak_ptr observes sp without increasing reference count

    std::cout << "  Observed use_count: " << wp.use_count() << "\n";

    // Convert weak_ptr to shared_ptr via lock() to safely access object
    if (auto locked_sp = wp.lock()) {
      locked_sp->doWork();
    }
  } // sp goes out of scope and destroys "ObservedRes"

  // Check if the observed object has been destroyed
  if (wp.expired()) {
    std::cout
        << "  weak_ptr is now expired (resource was safely destroyed).\n\n";
  }
}

int main() {
  demoUniquePtr();
  demoSharedPtr();
  demoWeakPtr();
  return 0;
}
