/// @brief Introduction to C++ unordered associative containers
///
/// Unordered associative containers are similar to ordered associative
/// containers. The main difference is that unordered associative containers are
/// implemented using hash tables rather than red-black trees (used by ordered
/// associative containers). Thus, unordered associative containers do not
/// require stored elements to have strict weak ordering (no operator<
/// required), but they do require a hash function and an equality operator
/// (operator==). Average time complexity for search, insertion, and deletion is
/// O(1).

#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

/// @brief Demonstrates basic operations of std::unordered_set.
///
/// Unique keys, elements are stored in arbitrary order defined by their hash
/// values.
void demoUnorderedSet() {
  std::cout << "=== std::unordered_set ===\n";

  std::unordered_set<std::string> set = {"apple", "banana", "cherry"};

  // Insert elements (duplicates are ignored)
  set.insert("date");
  set.insert("apple"); // Duplicate, will not be inserted

  // Search element
  if (set.find("banana") != set.end()) {
    std::cout << "Found 'banana' in set.\n";
  }

  // Erase element
  set.erase("cherry");

  // Traversal (Order is not guaranteed)
  std::cout << "Elements in set: ";
  for (const auto &item : set) {
    std::cout << item << " ";
  }
  std::cout << "\n\n";
}

/// @brief Demonstrates basic operations of std::unordered_multiset.
///
/// Duplicate keys are allowed. Useful when counting occurrences without
/// sorting.
void demoUnorderedMultiset() {
  std::cout << "=== std::unordered_multiset ===\n";

  std::unordered_multiset<std::string> multiset = {"apple", "banana", "apple"};

  // Insert duplicate elements
  multiset.insert("banana");
  multiset.insert("apple");

  // Count occurrences of an element
  std::cout << "Count of 'apple': " << multiset.count("apple") << "\n";

  // Traversal (Duplicates will be grouped near each other)
  std::cout << "Elements in multiset: ";
  for (const auto &item : multiset) {
    std::cout << item << " ";
  }
  std::cout << "\n\n";
}

/// @brief Demonstrates basic operations of std::unordered_map.
///
/// Key-value pairs with unique keys. Provides fast operator[] access.
void demoUnorderedMap() {
  std::cout << "=== std::unordered_map ===\n";

  std::unordered_map<std::string, int> map = {{"Alice", 25}, {"Bob", 30}};

  // Insertion and lookup using operator[]
  map["Charlie"] = 35;
  map["Alice"] = 26; // Overwrites existing value for key "Alice"

  // Safe search using find()
  auto it = map.find("Bob");
  if (it != map.end()) {
    std::cout << "Bob's age: " << it->second << "\n";
  }

  // Traversal
  std::cout << "Key-Value pairs in map:\n";
  for (const auto &[key, value] : map) {
    std::cout << "  " << key << " : " << value << "\n";
  }
  std::cout << "\n";
}

/// @brief Demonstrates basic operations of std::unordered_multimap.
///
/// Key-value pairs with duplicate keys allowed. Note: operator[] is NOT
/// supported.
void demoUnorderedMultimap() {
  std::cout << "=== std::unordered_multimap ===\n";

  std::unordered_multimap<std::string, std::string> multimap;

  // Insertion using insert() with std::make_pair or initializer list
  multimap.insert({"Genre", "Sci-Fi"});
  multimap.insert({"Genre", "Fantasy"});
  multimap.insert({"Author", "Asimov"});

  // Finding all values associated with a specific key
  std::cout << "Values under key 'Genre':\n";
  auto range = multimap.equal_range("Genre");
  for (auto it = range.first; it != range.second; ++it) {
    std::cout << "  " << it->first << " -> " << it->second << "\n";
  }

  // Traversal
  std::cout << "All Key-Value pairs in multimap:\n";
  for (const auto &[key, value] : multimap) {
    std::cout << "  " << key << " -> " << value << "\n";
  }
  std::cout << "\n";
}

int main() {
  demoUnorderedSet();
  demoUnorderedMultiset();
  demoUnorderedMap();
  demoUnorderedMultimap();

  return 0;
}
