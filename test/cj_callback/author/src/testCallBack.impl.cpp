#include "testCallBack.proj.hpp"
#include "testCallBack.impl.hpp"
#include "stdexcept"
using namespace taihe;

namespace {
// To be implemented.

void cb_void_void(::taihe::callback_view<void()> f) {
    f();
}

void cb_i_void(::taihe::callback_view<void(int32_t a)> f) {
    f(1);
}

::taihe::string cb_str_str(::taihe::callback_view<::taihe::string(::taihe::string_view a)> f) {
    taihe::string out = f("hello");
    return "hello" + out;
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_cb_void_void(cb_void_void);
TH_EXPORT_CPP_API_cb_i_void(cb_i_void);
TH_EXPORT_CPP_API_cb_str_str(cb_str_str);
// NOLINTEND
