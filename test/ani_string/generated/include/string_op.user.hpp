#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "string_op.proj.hpp"
#include "taihe/common.hpp"
#include "string_op.abi.h"
#include "taihe/string.hpp"
#include "string_op.StringPair.proj.2.hpp"
#include "taihe/array.hpp"
#include "string_op.PlayString.proj.2.hpp"
namespace string_op {
inline ::taihe::string concatString(::taihe::string_view a, ::taihe::string_view b) {
    return ::taihe::from_abi<::taihe::string>(string_op_concatString_f0(::taihe::into_abi<::taihe::string_view>(a), ::taihe::into_abi<::taihe::string_view>(b)));
}
}
namespace string_op {
inline ::taihe::string makeString(::taihe::string_view a, int32_t b) {
    return ::taihe::from_abi<::taihe::string>(string_op_makeString_f0(::taihe::into_abi<::taihe::string_view>(a), ::taihe::into_abi<int32_t>(b)));
}
}
namespace string_op {
inline ::string_op::StringPair split(::taihe::string_view a, int32_t n) {
    return ::taihe::from_abi<::string_op::StringPair>(string_op_split_f0(::taihe::into_abi<::taihe::string_view>(a), ::taihe::into_abi<int32_t>(n)));
}
}
namespace string_op {
inline ::taihe::array<::taihe::string> split2(::taihe::string_view a, int32_t n) {
    return ::taihe::from_abi<::taihe::array<::taihe::string>>(string_op_split2_f0(::taihe::into_abi<::taihe::string_view>(a), ::taihe::into_abi<int32_t>(n)));
}
}
namespace string_op {
inline int32_t to_i32(::taihe::string_view a) {
    return ::taihe::from_abi<int32_t>(string_op_to_i32_f02(::taihe::into_abi<::taihe::string_view>(a)));
}
}
namespace string_op {
inline ::taihe::string from_i32(int32_t a) {
    return ::taihe::from_abi<::taihe::string>(string_op_from_i32_f02(::taihe::into_abi<int32_t>(a)));
}
}
namespace string_op {
inline ::string_op::PlayString makePlayStringIface() {
    return ::taihe::from_abi<::string_op::PlayString>(string_op_makePlayStringIface_f0());
}
}
namespace string_op {
inline float to_f32(::taihe::string_view a) {
    return ::taihe::from_abi<float>(string_op_to_f32_f02(::taihe::into_abi<::taihe::string_view>(a)));
}
}
namespace string_op {
inline ::taihe::string from_f32(float a) {
    return ::taihe::from_abi<::taihe::string>(string_op_from_f32_f02(::taihe::into_abi<float>(a)));
}
}
namespace string_op {
inline ::taihe::string concatString2(::taihe::string_view s, int32_t n, ::taihe::array_view<::taihe::string> sArr, bool b, ::taihe::array_view<uint8_t> buffer) {
    return ::taihe::from_abi<::taihe::string>(string_op_concatString2_f0(::taihe::into_abi<::taihe::string_view>(s), ::taihe::into_abi<int32_t>(n), ::taihe::into_abi<::taihe::array_view<::taihe::string>>(sArr), ::taihe::into_abi<bool>(b), ::taihe::into_abi<::taihe::array_view<uint8_t>>(buffer)));
}
}
#pragma clang diagnostic pop
