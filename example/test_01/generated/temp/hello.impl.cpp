#include "hello.proj.hpp"
#include "hello.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"


namespace {
// To be implemented.

void sayHello() {
    TH_THROW(std::runtime_error, "sayHello not implemented");
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_sayHello(sayHello);
// NOLINTEND
