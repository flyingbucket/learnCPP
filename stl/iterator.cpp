#include <array>
#include <forward_list>
#include <iostream>
#include <iterator>
#include <list>
#include <sstream>
#include <vector>

/// @brief Demonstrates the properties and usage of an Input Iterator.
///
/// Properties of Input Iterators:
/// - Direction: Forward-only, single-pass traversal.
/// - Access: Read-only (can dereference *it to access value on RHS).
/// - Mutability: Non-assignable via dereference (*it = x is INVALID).
/// - Operations: ++it, it++, *it, it->, ==, !=
/// - Invalidation: Incrementing invalidates copies of previous iterator
/// positions.
/// - Standard Example: std::istream_iterator
void demo_InputIterator() {
  std::cout << "=== 1. Input Iterator Example ===\n";
  std::string data = "10 20 30 40 50";
  std::istringstream stream(data);

  // std::istream_iterator models an Input Iterator reading from an input stream
  std::istream_iterator<int> inputIt(stream);
  std::istream_iterator<int>
      endIt; // Default constructor represents end-of-stream

  std::cout << "Reading stream values sequentially: ";
  while (inputIt != endIt) {
    std::cout << *inputIt << " "; // Read value (RHS dereference)
    ++inputIt;                    // Advance to next element (single-pass)
  }
  std::cout << "\n\n";
}

/// @brief Demonstrates the properties and usage of an Output Iterator.
///
/// Properties of Output Iterators:
/// - Direction: Forward-only, single-pass traversal.
/// - Access: Write-only (can dereference *it on LHS of assignment).
/// - Mutability: Values can be written, but reading (*it) is NOT guaranteed.
/// - Operations: ++it, it++, *it = value
/// - Standard Example: std::ostream_iterator, std::back_insert_iterator
void demon_OutputIterator() {
  std::cout << "=== 2. Output Iterator Example ===\n";
  std::vector<int> source = {1, 2, 3, 4, 5};

  // std::ostream_iterator models an Output Iterator writing directly to
  // std::cout
  std::ostream_iterator<int> outputIt(std::cout, " ");

  std::cout << "Writing vector elements via std::ostream_iterator: ";
  for (int val : source) {
    *outputIt = val; // Write value (LHS dereference)
    ++outputIt;      // Advance iterator
  }
  std::cout << "\n\n";
}

/// @brief Demonstrates the properties and usage of a Forward Iterator.
///
/// Properties of Forward Iterators:
/// - Extends: Input Iterator & Output Iterator features.
/// - Traversal: Forward-only, multi-pass (can save position and re-traverse).
/// - Access: Read and Write allowed (unless const_iterator).
/// - Operations: ++it, it++, *it, it->, ==, !=, default constructible.
/// - Standard Example: std::forward_list::iterator,
/// std::unordered_map::iterator
void demon_ForwardIterator() {
  std::cout << "=== 3. Forward Iterator Example ===\n";
  std::forward_list<int> flist = {10, 20, 30, 40};

  // Multi-pass capability: save an iterator position for later reuse
  auto savedIt = flist.begin(); // Points to 10

  std::cout << "First pass (multiplying values by 2): ";
  for (auto it = flist.begin(); it != flist.end(); ++it) {
    *it *= 2; // Read & Write allowed
    std::cout << *it << " ";
  }
  std::cout << "\n";

  // We can still safely dereference savedIt because Forward Iterators support
  // multi-pass
  std::cout << "Value at saved iterator position: " << *savedIt << "\n\n";
}

/// @brief Demonstrates the properties and usage of a Bidirectional Iterator.
///
/// Properties of Bidirectional Iterators:
/// - Extends: Forward Iterator.
/// - Traversal: Both forward (++it) and backward (--it) movement.
/// - Access: Read and Write allowed.
/// - Operations: All Forward Iterator operations plus --it, it--.
/// - Standard Example: std::list::iterator, std::set::iterator,
/// std::map::iterator
void demon_BidirectionalIterator() {
  std::cout << "=== 4. Bidirectional Iterator Example ===\n";
  std::list<int> lst = {100, 200, 300, 400};

  std::cout << "Traversing forward: ";
  for (auto it = lst.begin(); it != lst.end(); ++it) {
    std::cout << *it << " ";
  }
  std::cout << "\n";

  std::cout << "Traversing backward using decrement (--): ";
  auto it = lst.end();
  while (it != lst.begin()) {
    --it; // Move backward before dereferencing since end() points past the last
          // element
    std::cout << *it << " ";
  }
  std::cout << "\n\n";
}

/// @brief Demonstrates the properties and usage of a Random Access Iterator.
///
/// Properties of Random Access Iterators:
/// - Extends: Bidirectional Iterator.
/// - Traversal: Arbitrary constant-time O(1) positioning.
/// - Arithmetic: it + n, it - n, it += n, it -= n, it2 - it1 (distance).
/// - Subscripting: it[n] (equivalent to *(it + n)).
/// - Comparisons: Relational operators (<, <=, >, >=).
/// - Standard Example: std::vector::iterator, std::deque::iterator
void demo_RandomAccessIterator() {
  std::cout << "=== 5. Random Access Iterator Example ===\n";
  std::vector<int> vec = {10, 20, 30, 40, 50, 60};

  auto it = vec.begin();

  std::cout << "First element (*it): " << *it << "\n";
  std::cout << "Jump forward 3 positions (*(it + 3)): " << *(it + 3) << "\n";
  std::cout << "Subscript access (it[4]): " << it[4] << "\n";

  auto midIt = vec.begin() + 3;
  auto endIt = vec.end();

  // Iterator distance calculation in O(1)
  std::cout << "Distance from mid to end (endIt - midIt): " << (endIt - midIt)
            << " elements\n";
  std::cout << "Relational check (midIt < endIt): "
            << (midIt < endIt ? "True" : "False") << "\n\n";
}

/// @brief Demonstrates the properties and usage of a Contiguous Iterator.
///
/// Properties of Contiguous Iterators:
/// - Extends: Random Access Iterator.
/// - Storage Guarantee: Elements are guaranteed to be stored contiguously in
/// memory.
/// - Pointer Compatibility: Expression &(*(it + n)) is equivalent to (&(*it) +
/// n).
/// - Operations: All Random Access Iterator operations.
/// - Standard Example: std::array::iterator, std::vector::iterator, Raw
/// Pointers
void demo_ContiguousIterator() {
  std::cout << "=== 6. Contiguous Iterator Example ===\n";
  std::array<int, 4> arr = {10, 20, 30, 40};

  auto it = arr.begin();

  const int *rawPtrFirst = &(*it);
  const int *rawPtrThird = &(*(it + 2));

  std::cout << "Address of 1st element (&(*it)):     "
            << static_cast<const void *>(rawPtrFirst) << "\n";
  std::cout << "Address of 3rd element (&(*(it+2))): "
            << static_cast<const void *>(rawPtrThird) << "\n";
  std::cout << "Pointer offset calculation:          "
            << (rawPtrThird - rawPtrFirst) << " elements\n\n";
}

int main() {
  demo_InputIterator();
  demon_OutputIterator();
  demon_ForwardIterator();
  demon_BidirectionalIterator();
  demo_RandomAccessIterator();
  demo_ContiguousIterator();

  return 0;
}
