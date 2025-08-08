#include "string_io.proj.hpp"
#include "string_io.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"


namespace {
// To be implemented.

::taihe::string input() {
    TH_THROW(std::runtime_error, "input not implemented");
}

void print(::taihe::string_view a) {
    TH_THROW(std::runtime_error, "print not implemented");
}

void println(::taihe::string_view a) {
    TH_THROW(std::runtime_error, "println not implemented");
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_input(input);
TH_EXPORT_CPP_API_print(print);
TH_EXPORT_CPP_API_println(println);
// NOLINTEND
