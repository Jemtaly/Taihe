#include "test_struct.proj.hpp"
#include "test_struct.impl.hpp"
#include "stdexcept"

::test_struct::Color getNewColor(::test_struct::Color const& origin) {
    ::test_struct::Color res;
    res.r = origin.r + 1;
    res.g = origin.g + 2;
    res.b = origin.b + 3;
    return res;
}

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_getNewColor(getNewColor);
// NOLINTEND
