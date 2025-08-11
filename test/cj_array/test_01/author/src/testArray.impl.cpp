#include "testArray.proj.hpp"
#include "testArray.impl.hpp"
#include "numeric"
#include "stdexcept"

using namespace taihe;

int32_t SumArray(array_view<int32_t> nums, int32_t base) {
  return std::accumulate(nums.begin(), nums.end(), base);
}

int64_t GetArrayValue(array_view<int64_t> nums, int32_t idx) {
  if (idx >= 0 && idx < nums.size()) {
    return nums[idx];
  }
  throw std::runtime_error("Index out of range");
}

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_SumArray(SumArray);
TH_EXPORT_CPP_API_GetArrayValue(GetArrayValue);
// NOLINTEND
