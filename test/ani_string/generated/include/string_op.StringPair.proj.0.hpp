#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe/common.hpp"
#include "string_op.StringPair.abi.0.h"
namespace string_op {
struct StringPair;
}
namespace taihe {
template<>
struct as_abi<::string_op::StringPair> {
    using type = struct string_op_StringPair_t0;
};
template<>
struct as_abi<::string_op::StringPair const&> {
    using type = struct string_op_StringPair_t0 const*;
};
template<>
struct as_param<::string_op::StringPair> {
    using type = ::string_op::StringPair const&;
};
}
#pragma clang diagnostic pop
