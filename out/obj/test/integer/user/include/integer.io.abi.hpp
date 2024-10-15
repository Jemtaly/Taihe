#pragma once
#include <taihe/common.hpp>
#include <integer.io.abi.h>
namespace integer::io {
inline int32_t input_i32() {
    return taihe::core::from_abi<int32_t, int32_t>(__integer__io__input_i32());
}
}
namespace integer::io {
inline void output_i32(int32_t n) {
    return (__integer__io__output_i32(taihe::core::into_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(n))));
}
}
