#include "testOptional.proj.hpp"
#include "testOptional.impl.hpp"
#include <iostream>
#include "stdexcept"
using namespace taihe;

void ShowOptionalInt(optional_view<int32_t> x) {
  if (x) {
    std::cout << *x << std::endl;
  } else {
    std::cout << "Null" << std::endl;
  }
}

optional<int32_t> MakeOptionalInt(bool b) {
  if (b) {
    int const optionalMakeValue = 10;
    return optional<int32_t>::make(optionalMakeValue);
  } else {
    return optional<int32_t>(nullptr);
  }
}

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_ShowOptionalInt(ShowOptionalInt);
TH_EXPORT_CPP_API_MakeOptionalInt(MakeOptionalInt);
// NOLINTEND
