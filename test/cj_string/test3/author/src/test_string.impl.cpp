#include "test_string.proj.hpp"
#include "test_string.impl.hpp"

#include "stdexcept"


namespace {
// To be implemented.

::taihe::string getNewString(::test_string::in_string const& origin) {
   return origin.s ;
}  // namespace
}
// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_getNewString(getNewString);
// NOLINTEND
