#include <cstddef>
#include <cstdlib>
#include <iostream>

#include "stack/ShuntingYard.hpp"

int main() {
  size_t required_size = infix_to_postfix("(a+b)*(c+d)", NULL);
  char* res_buff = (char*)malloc(sizeof(char) * required_size);
  size_t actuall_size = infix_to_postfix("(a+b)*(c+d)", res_buff);
  std::cout << res_buff << std::endl;
  return 0;
}
