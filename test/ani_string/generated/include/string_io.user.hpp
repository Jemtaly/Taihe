#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "string_io.proj.hpp"
#include "taihe/common.hpp"
#include "string_io.abi.h"
#include "taihe/string.hpp"
namespace string_io {
inline ::taihe::string input() {
    return ::taihe::from_abi<::taihe::string>(string_io_input_f0());
}
}
namespace string_io {
inline void print(::taihe::string_view a) {
    return string_io_print_f0(::taihe::into_abi<::taihe::string_view>(a));
}
}
namespace string_io {
inline void println(::taihe::string_view a) {
    return string_io_println_f0(::taihe::into_abi<::taihe::string_view>(a));
}
}
#pragma clang diagnostic pop
