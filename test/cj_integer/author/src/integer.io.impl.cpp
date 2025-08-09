#include "integer.io.impl.hpp"

#include <iostream>

int32_t input_i32() {
  int n;
  std::cin >> n;
  return n;
}

void output_i32(int32_t n) {
  std::cout << n << std::endl;
}

TH_EXPORT_CPP_API_input_i32(input_i32);
TH_EXPORT_CPP_API_output_i32(output_i32);
