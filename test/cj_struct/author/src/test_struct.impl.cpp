#include "test_struct.proj.hpp"
#include "test_struct.impl.hpp"

#include "stdexcept"


namespace {
// To be implemented.

::test_struct::Color getNewColor(::test_struct::Color const& origin, ::test_struct::Color const& another) {
    ::test_struct::Color res;
    res.r = origin.r + another.r;
    res.g = origin.g + another.g;
    res.b = origin.b + another.b;
    return res;
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_getNewColor(getNewColor);
// NOLINTEND
