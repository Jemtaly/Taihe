#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe/object.hpp"
#include "string_op.PlayString.abi.0.h"
namespace string_op::weak {
struct PlayString;
}
namespace string_op {
struct PlayString;
}
namespace taihe {
template<>
struct as_abi<::string_op::PlayString> {
    using type = struct string_op_PlayString_t0;
};
template<>
struct as_abi<::string_op::weak::PlayString> {
    using type = struct string_op_PlayString_t0;
};
template<>
struct as_param<::string_op::PlayString> {
    using type = ::string_op::weak::PlayString;
};
}
#pragma clang diagnostic pop
