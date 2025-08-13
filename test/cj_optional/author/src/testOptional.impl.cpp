#include "testOptional.proj.hpp"
#include "testOptional.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"


namespace {
// To be implemented.

void ShowOptionalInt(::taihe::optional_view<int32_t> x) {
    TH_THROW(std::runtime_error, "ShowOptionalInt not implemented");
}

::taihe::optional<int32_t> MakeOptionalInt(bool b) {
    TH_THROW(std::runtime_error, "MakeOptionalInt not implemented");
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_ShowOptionalInt(ShowOptionalInt);
TH_EXPORT_CPP_API_MakeOptionalInt(MakeOptionalInt);
// NOLINTEND
