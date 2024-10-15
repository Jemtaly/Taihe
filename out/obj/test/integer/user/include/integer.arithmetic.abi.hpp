#pragma once
#include <taihe/common.hpp>
#include <integer.arithmetic.abi.h>
namespace integer::arithmetic {
inline int32_t add_i32(int32_t a, int32_t b) {
    return taihe::core::from_abi<int32_t, int32_t>(__integer__arithmetic__add_i32(taihe::core::into_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(a)), taihe::core::into_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(b))));
}
}
namespace integer::arithmetic {
inline int32_t sub_i32(int32_t a, int32_t b) {
    return taihe::core::from_abi<int32_t, int32_t>(__integer__arithmetic__sub_i32(taihe::core::into_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(a)), taihe::core::into_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(b))));
}
}
namespace integer::arithmetic {
inline int32_t mul_i32(int32_t a, int32_t b) {
    return taihe::core::from_abi<int32_t, int32_t>(__integer__arithmetic__mul_i32(taihe::core::into_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(a)), taihe::core::into_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(b))));
}
}
template<>
inline std::tuple<int32_t, int32_t> taihe::core::from_abi(std::add_rvalue_reference_t<struct __integer__arithmetic__divmod_i32__return_t> _val) {
    return {
        taihe::core::from_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(_val._0)), 
        taihe::core::from_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(_val._1)), 
    };
}
namespace integer::arithmetic {
inline std::tuple<int32_t, int32_t> divmod_i32(int32_t a, int32_t b) {
    return taihe::core::from_abi<std::tuple<int32_t, int32_t>, struct __integer__arithmetic__divmod_i32__return_t>(__integer__arithmetic__divmod_i32(taihe::core::into_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(a)), taihe::core::into_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(b))));
}
}
