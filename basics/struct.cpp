#include <concepts>
#include <cstddef>
#include <iostream>
#include <string>

/// In cpp, Struct is basicly identital to Class. You can implement all kinds of
/// methods in a Struct.
/// Struct also supports inheritance and polymorphism.
/// The only difference is that Structs are public by default while Classes are
/// private by default.
/// Here is an example:

// Dynamic Polymorphism Interface
struct Printable {
  // Pure Virtual Function:
  //   - Defined by appending `= 0` to a virtual function signature.
  //   - Makes Printable an Abstract Class (cannot be instantiated directly).
  //   - Forces derived structs/classes to implement this method.
  //   - Defines a uniform contract for runtime polymorphism.
  virtual void printInfo() const = 0;

  // Virtual Destructor:
  // Ensures derived class destructors are properly invoked during deletion
  // through a base pointer.
  virtual ~Printable() = default;
};

// Derived struct implementing the interface
struct Person : public Printable {
  std::string name;
  int age;

  // constructor
  Person(std::string n, int a) : name(n), age(a) {}

  // overriding the pure virtual function
  void printInfo() const override {
    std::cout << "[Person] Name: " << name << ", Age: " << age << std::endl;
  }
};

// Inherite form Struct Person
struct Student : public Person {
  int studentId;

  Student(std::string n, int a, int id) : Person(n, a), studentId(id) {}

  void printInfo() const override {
    std::cout << "[Student] Name: " << name << ", Age: " << age
              << ", ID: " << studentId << std::endl;
  }
};

template <typename T>
concept CanPrint = requires(T a) {
  { a.printInfo() } -> std::same_as<void>;
};

void render(const CanPrint auto &item) { item.printInfo(); }

// --- Memory Layout & Alignment ---

struct UnalignedLayout {
  char a; // 1 byte
          // [Padding: 3 bytes] to aligh with the following int
  int b;  // 4 bytes
  char c; // 1 byte
          // [Padding: 3 bytes] The size of a struct is an integer multiple of
          // the size of its largest member.
}; // Total: 1 + 3 + 4 + 1 + 3 = 12 bytes

struct AlignedLayout {
  int b;  // 4 bytes
  char a; // 1 byte
  char c; // 1 byte
          // [Padding: 2 bytes]
}; // Total: 4 + 1 + 1 + 2 = 8 bytes

// Memory Layout with virtual methods
struct VirtualLayout {
  // [vptr: 8 bytes]
  int id; // 4 bytes
          // [Padding: 4 bytes]
}; // Total size under 64bits system: 8 + 4 + 4 = 16 bytes

int main() {
  std::cout << "=== Inheritance and Polymorphism ===" << std::endl;
  Person p("Jack", 25);
  Student s("Bob", 20, 1001);

  // Polymorphism through interface
  Printable *ptr1 = &p;
  Printable *ptr2 = &s;
  ptr1->printInfo();
  ptr2->printInfo();

  std::cout << "\n=== C++20 Concept ===" << std::endl;
  render(p);
  render(s);

  std::cout << "\n=== Memory Layout ===" << std::endl;
  std::cout << "sizeof(UnalignedLayout): " << sizeof(UnalignedLayout)
            << " bytes" << std::endl;
  std::cout << "  - offset of 'a': " << offsetof(UnalignedLayout, a) << " byte"
            << std::endl;
  std::cout << "  - offset of 'b': " << offsetof(UnalignedLayout, b)
            << " bytes (with 3 bytes padding)" << std::endl;
  std::cout << "  - offset of 'c': " << offsetof(UnalignedLayout, c) << " bytes"
            << std::endl;

  std::cout << "\nsizeof(AlignedLayout): " << sizeof(AlignedLayout)
            << " bytes (rearranged)" << std::endl;

  std::cout << "\nsizeof(VirtualLayout): " << sizeof(VirtualLayout) << " bytes"
            << std::endl;
  std::cout
      << "  - offset of 'id': " << offsetof(VirtualLayout, id)
      << " bytes (first 8 bytes are vptr for virtual method pointer table)"
      << std::endl;

  return 0;
}
