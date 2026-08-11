#include <iostream>
#include <map>
#include <set>
#include <string>

/// @brief Demonstrates the usage of std::set.
///
/// Interpretation & Explanation:
/// std::set is an associative container that contains a sorted set of unique
/// objects of type Key. It is usually implemented as a red-black tree.
///
/// Key characteristics shown in this function:
/// 1. Automatic Sorting: Elements are automatically sorted in ascending order
/// upon insertion.
///
/// 2. Uniqueness: Attempting to insert a duplicate value (like inserting 20
/// twice) will be ignored.
/// The container ensures all elements are unique.
///
/// 3. Efficient Lookup: We use the .find() method to search for an element,
/// which operates in O(log n) time complexity, making it highly efficient
/// for large datasets.
void demonstrate_set() {
  std::cout << "--- Demonstrating std::set ---" << std::endl;

  // Create a set of integers
  std::set<int> my_set;

  // Insert elements into the set
  my_set.insert(40);
  my_set.insert(10);
  my_set.insert(30);
  my_set.insert(20);

  // Attempt to insert a duplicate element (will be ignored)
  my_set.insert(20);

  // Iterate through the set using a range-based for loop
  // Notice that the output will be sorted: 10, 20, 30, 40
  std::cout << "Elements in set (automatically sorted and unique): ";
  for (int num : my_set) {
    std::cout << num << " ";
  }
  std::cout << std::endl;

  // Check if a specific element exists in the set
  int target = 30;
  if (my_set.find(target) != my_set.end()) {
    std::cout << "Element " << target << " found in the set." << std::endl;
  } else {
    std::cout << "Element " << target << " not found." << std::endl;
  }
  std::cout << std::endl;
}

/// @brief Demonstrates the usage of std::map.
///
/// Interpretation & Explanation:
/// std::map is a sorted associative container that contains key-value pairs
/// with unique keys. Keys are sorted automatically using the comparison
/// function (default is std::less).
///
/// Key characteristics shown in this function:
/// 1. Key-Value Association: Each element is a std::pair consisting of a const
/// Key and a Value.
///
/// 2. Operator[]: The subscript operator provides a convenient way to insert
/// new key-value pairs or update the value of an existing key.
/// If the key does not exist, it creates a new entry with a
/// default-constructed value before assigning the new value.
///
/// 3. Iteration: When iterating, we access the key using `it->first` and the
/// value using `it->second`.
void demonstrate_map() {
  std::cout << "--- Demonstrating std::map ---" << std::endl;

  // Create a map where the key is a string (Name) and the value is an integer
  // (Age)
  std::map<std::string, int> age_map;

  // Insert elements using the subscript operator []
  age_map["Alice"] = 28;
  age_map["Charlie"] = 35;
  age_map["Bob"] = 22;

  // Update an existing value
  // Charlie's age will be updated from 35 to 36
  age_map["Charlie"] = 36;

  // Insert element using the insert() method with std::make_pair
  age_map.insert(std::make_pair("Diana", 30));

  // Iterate through the map
  // Notice that the output is sorted alphabetically by the Key (Name)
  std::cout << "Elements in map (sorted by Key): " << std::endl;
  for (auto const &pair : age_map) {
    std::cout << pair.first << " is " << pair.second << " years old."
              << std::endl;
  }

  // Access an element safely using find()
  // It prevents accidental insertion of a default value which can happen with
  // operator[]
  std::string search_name = "Bob";
  auto it = age_map.find(search_name);
  if (it != age_map.end()) {
    std::cout << "Found " << it->first << ", age: " << it->second << std::endl;
  } else {
    std::cout << search_name << " not found in the map." << std::endl;
  }
  std::cout << std::endl;
}

/// @brief Demonstrates the usage of std::multiset.
///
/// Interpretation & Explanation:
/// std::multiset is an associative container similar to std::set, but it allows
/// multiple elements to have the same equivalent value (duplicate keys).
///
/// Key characteristics shown in this function:
/// 1. Duplicates Allowed: Inserting the same value multiple times is permitted.
/// 2. Ordered Sequence: All elements, including duplicates, are sorted
/// automatically.
/// 3. Counting & Range: The .count() method is highly useful here to see how
/// many
///    copies of a value exist. We also demonstrate .equal_range() which returns
///    the bounds (iterators) containing all duplicate values of a specific key.
void demonstrate_multiset() {
  std::cout << "--- Demonstrating std::multiset ---" << std::endl;

  // Create a multiset of integers
  std::multiset<int> my_multiset;

  // Insert elements, including duplicates
  my_multiset.insert(30);
  my_multiset.insert(10);
  my_multiset.insert(20);
  my_multiset.insert(20); // Duplicate
  my_multiset.insert(40);
  my_multiset.insert(20); // duplicate

  // Iterate through the multiset
  // Notice the output is sorted and retains all duplicates: 10, 20, 20, 20, 30,
  // 40
  std::cout << "Elements in multiset: ";
  for (int num : my_multiset) {
    std::cout << num << " ";
  }
  std::cout << std::endl;

  // Count how many times the value '20' appears
  int target = 20;
  std::cout << "The value " << target << " appears "
            << my_multiset.count(target) << " times." << std::endl;

  // Erase a specific element
  // Note: Calling erase(20) on a multiset will remove ALL instances of 20 by
  // default! If you only want to remove one instance, you must pass an
  // iterator: my_multiset.erase(my_multiset.find(20));
  my_multiset.erase(target);
  std::cout << "After erasing " << target << ", elements: ";
  for (int num : my_multiset) {
    std::cout << num << " ";
  }
  std::cout << "\n\n";
}

/// @brief Demonstrates the usage of std::multimap.
///
/// Interpretation & Explanation:
/// std::multimap is an associative container that contains sorted list of
/// key-value pairs, while permitting multiple entries with the same key.
///
/// Key characteristics shown in this function:
/// 1. No Operator[]: You cannot use `map[key] = value` because a single key
/// might
///    point to multiple values. You must use .insert() or .emplace().
/// 2. Grouping Data: It is excellent for representing 1-to-N relationships,
/// such as
///    mapping a "Department" to multiple "Employees".
/// 3. Fetching Duplicates: We use .equal_range(key) to retrieve a pair of
/// iterators
///    that represent the beginning and the end of all values associated with
///    that key.
void demonstrate_multimap() {
  std::cout << "--- Demonstrating std::multimap ---" << std::endl;

  // Create a multimap grouping employees by department
  // Key: Department Name (string), Value: Employee Name (string)
  std::multimap<std::string, std::string> dept_employees;

  // Insert elements using insert() or emplace()
  dept_employees.insert(std::make_pair("Engineering", "Alice"));
  dept_employees.insert(std::make_pair("Sales", "Bob"));
  dept_employees.insert(
      std::make_pair("Engineering", "Charlie")); // Duplicate key
  dept_employees.insert(std::make_pair("HR", "Diana"));
  dept_employees.insert(std::make_pair("Engineering", "Eve")); // Duplicate key

  // Iterate through all elements
  // Output is sorted by Department Name (the Key)
  std::cout << "All employees grouped by department:" << std::endl;
  for (auto const &pair : dept_employees) {
    std::cout << "[" << pair.first << "] " << pair.second << std::endl;
  }
  std::cout << std::endl;

  // Find and print all employees in the "Engineering" department
  std::string search_dept = "Engineering";
  std::cout << "Searching for all employees in " << search_dept << ":"
            << std::endl;

  // equal_range returns a std::pair of iterators:
  // .first is the start of the range, .second is the end of the range
  auto range = dept_employees.equal_range(search_dept);

  // Iterate over the specific range for the duplicated keys
  for (auto it = range.first; it != range.second; it++) {
    std::cout << "- " << it->second << std::endl;
  }
  std::cout << std::endl;
}

int main() {
  demonstrate_set();
  demonstrate_map();
  demonstrate_multiset();
  demonstrate_multimap();
  return 0;
}
