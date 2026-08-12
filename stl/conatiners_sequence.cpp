/// @brief Introduction to C++ Sequence Containers
///
/// Sequence containers implement data structures that can be accessed
/// sequentially. They store elements of the same type in a linear arrangement.
/// The standard library provides five primary sequence containers:
/// 1. std::array: Fixed-size contiguous array.
/// 2. std::vector: Dynamically-sized contiguous array.
/// 3. std::deque: Double-ended queue.
/// 4. std::list: Doubly-linked list.
/// 5. std::forward_list: Singly-linked list.
///
/// Below are individual demonstrations for each container.

#include <array>
#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <ostream>
#include <vector>

// @brief Demonstrates std::array
//
// - Underlying Structure: Fixed-size contiguous memory (Static Array).
// - Performance:
//   - Random access: O(1)
//   - Insertion/Deletion: Not supported (size is fixed at compile time).
// - Use Case: When you know the exact number of elements at compile time
//   and need zero memory overhead with maximum performance.
void demo_array() {
  std::cout << "--- std::array Demo ---\n";

  // Initialize an array of 5 integers
  std::array<int, 5> arr = {10, 20, 30, 40, 50};

  // Modification using random access
  arr[2] = 99; // Changes the third element

  std::cout << "Array contents: ";
  for (const auto &elem : arr) {
    std::cout << elem << " ";
  }
  std::cout << "\n\n";
}

// @brief Demonstrates std::vector
//
// - Underlying Structure: Dynamically allocated contiguous memory (Dynamic
// Array).
// - Performance:
//   - Random access: O(1)
//   - Insertion/Deletion at the end: Amortized O(1)
//   - Insertion/Deletion in the middle/front: O(N) because elements must be
// shifted.
// - Use Case: Default choice for most sequence container needs. Excellent cache
//   locality and fast iteration.
void demo_vector() {
  std::cout << "--- std::vector Demo ---\n";

  std::vector<int> vec = {1, 2, 3};

  // info
  std::cout << "vec size: " << vec.size() << std::endl;
  std::cout << "vec capacity: " << vec.capacity() << std::endl;
  std::cout << "is vec empty?: " << vec.empty() << std::endl;

  // get element by index
  std::cout << "element at index 1: " << vec.at(1) << std::endl;

  // reserve memory for the vector for future push, vector size stay the same
  vec.reserve(50);

  // Dynamic resizing
  vec.push_back(4);
  vec.push_back(5);

  // release unused memory
  vec.shrink_to_fit();

  vec.pop_back(); // Removes the last element (5)

  // Random access
  vec[0] = 100;

  std::cout << "Vector contents: ";
  for (const auto &elem : vec) {
    std::cout << elem << " ";
  }
  std::cout << "\n\n";
}

// @brief Demonstrates std::deque (Double-Ended Queue)
//
// - Underlying Structure: A sequence of individually allocated fixed-size
// arrays.
// - Performance:
//   - Random access: O(1) (slightly slower than vector due to pointer math).
//   - Insertion/Deletion at front and back: O(1)
//   - Insertion/Deletion in the middle: O(N)
// - Use Case: When you need fast insertions and deletions at BOTH ends of the
//   sequence (e.g., a queue or scheduling system).
void demo_deque() {
  std::cout << "--- std::deque Demo ---\n";

  std::deque<int> dq = {20, 30};

  // Fast insertion at both ends
  dq.push_front(10);
  dq.push_back(40);

  std::cout << "Deque contents: ";
  for (const auto &elem : dq) {
    std::cout << elem << " ";
  }
  std::cout << "\n\n";
}

// @brief Demonstrates std::list
//
// - Underlying Structure: Doubly-linked list.
// - Performance:
//   - Random access: Not supported (must traverse sequentially, O(N)).
//   - Insertion/Deletion anywhere: O(1) (if you already have the iterator).
// - Use Case: When frequent insertions and deletions from the middle of the
//   sequence are required, and random access is not needed.
void demo_list() {
  std::cout << "--- std::list Demo ---\n";

  std::list<int> lst = {20, 30, 40};

  lst.push_front(10); // O(1) insertion at the front
  lst.push_back(50);  // O(1) insertion at the back

  // Inserting in the middle requires an iterator
  auto it = lst.begin();
  std::advance(it, 2); // Move iterator to point to 30
  lst.insert(it, 25);  // Inserts 25 before 30. O(1) insertion.

  std::cout << "List contents: ";
  for (const auto &elem : lst) {
    std::cout << elem << " ";
  }
  std::cout << "\n\n";
}

// @brief Demonstrates std::forward_list
//
// - Underlying Structure: Singly-linked list.
// - Performance:
//   - Random access: Not supported.
//   - Insertion/Deletion: O(1) but only AFTER a given node (insert_after).
//   - Iteration: Forward only.
// - Use Case: When memory overhead must be kept to an absolute minimum and
//   you only need to traverse the data sequentially in one direction.
//   (Comparable to a manual C-style singly linked list).
void demo_forward_list() {
  std::cout << "--- std::forward_list Demo ---\n";

  std::forward_list<int> flst = {20, 30, 40};

  // Only push_front is supported, push_back is not.
  flst.push_front(10);

  // To insert in the middle, we must use insert_after
  auto it = flst.begin();
  flst.insert_after(it, 15); // Inserts 15 right after the first element (10)

  std::cout << "Forward_list contents: ";
  for (const auto &elem : flst) {
    std::cout << elem << " ";
  }
  std::cout << "\n\n";
}

int main() {
  std::cout << "========================================\n";
  std::cout << " C++ Sequence Containers Demonstration \n";
  std::cout << "========================================\n\n";

  demo_array();
  demo_vector();
  demo_deque();
  demo_list();
  demo_forward_list();

  return 0;
}
