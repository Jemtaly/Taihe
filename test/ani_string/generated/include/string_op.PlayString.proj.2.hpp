#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "string_op.PlayString.proj.1.hpp"
#include "string_op.PlayString.abi.2.h"
#include "taihe/array.hpp"
#include "taihe/string.hpp"
struct ::string_op::weak::PlayString::virtual_type {
    ::taihe::string pickString(::taihe::array_view<::taihe::string> nums, int32_t n1, int32_t n2) const& {
        return ::taihe::from_abi<::taihe::string>(string_op_PlayString_pickString_f0(*reinterpret_cast<string_op_PlayString_t0 const*>(this), ::taihe::into_abi<::taihe::array_view<::taihe::string>>(nums), ::taihe::into_abi<int32_t>(n1), ::taihe::into_abi<int32_t>(n2)));
    }
    ::taihe::string getName() const& {
        return ::taihe::from_abi<::taihe::string>(string_op_PlayString_getName_f0(*reinterpret_cast<string_op_PlayString_t0 const*>(this)));
    }
    void setName(::taihe::string_view name) const& {
        return string_op_PlayString_setName_f0(*reinterpret_cast<string_op_PlayString_t0 const*>(this), ::taihe::into_abi<::taihe::string_view>(name));
    }
};
template<typename Impl>
struct ::string_op::weak::PlayString::methods_impl {
    static struct TString pickString(struct string_op_PlayString_t0 tobj, struct TArray nums, int32_t n1, int32_t n2) {
        return ::taihe::into_abi<::taihe::string>(::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->pickString(::taihe::from_abi<::taihe::array_view<::taihe::string>>(nums), ::taihe::from_abi<int32_t>(n1), ::taihe::from_abi<int32_t>(n2)));
    }
    static struct TString getName(struct string_op_PlayString_t0 tobj) {
        return ::taihe::into_abi<::taihe::string>(::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->getName());
    }
    static void setName(struct string_op_PlayString_t0 tobj, struct TString name) {
        return ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->setName(::taihe::from_abi<::taihe::string_view>(name));
    }
};
template<typename Impl>
constexpr string_op_PlayString_ftable0 string_op::weak::PlayString::ftbl_impl = {
    .pickString = &methods_impl<Impl>::pickString,
    .getName = &methods_impl<Impl>::getName,
    .setName = &methods_impl<Impl>::setName,
};
#pragma clang diagnostic pop
