#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe/platform/ani.hpp"
#include "string_op.PlayString.proj.1.hpp"
template<> struct ::taihe::from_ani_t<::string_op::PlayString> {
    inline ::string_op::PlayString operator()(ani_env* env, ani_object ani_obj) const;
};
template<> struct ::taihe::into_ani_t<::string_op::PlayString> {
    inline ani_object operator()(ani_env* env, ::string_op::PlayString cpp_obj) const;
};
#pragma clang diagnostic pop
