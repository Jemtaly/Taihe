#include "integer.arithmetic.impl.hpp"

int32_t add_i32(int32_t a, int32_t b) {
  return a + b;
}

int32_t sub_i32(int32_t a, int32_t b) {
  return a - b;
}

int32_t mul_i32(int32_t a, int32_t b) {
  return a * b;
}

TH_EXPORT_CPP_API_add_i32(add_i32);
TH_EXPORT_CPP_API_sub_i32(sub_i32);
TH_EXPORT_CPP_API_mul_i32(mul_i32);
