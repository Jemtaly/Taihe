#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "string_op.PlayString.proj.0.hpp"
#include "string_op.PlayString.abi.1.h"
namespace string_op::weak {
struct PlayString {
    static constexpr bool is_holder = false;
    struct string_op_PlayString_t0 m_handle;
    explicit PlayString(struct string_op_PlayString_t0 handle) : m_handle(handle) {}
    operator ::taihe::data_view() const& {
        return ::taihe::data_view(this->m_handle.data_ptr);
    }
    operator ::taihe::data_holder() const& {
        return ::taihe::data_holder(tobj_dup(this->m_handle.data_ptr));
    }
    explicit PlayString(::taihe::data_view other) : PlayString({
        string_op_PlayString_dynamic0(other.data_ptr->rtti_ptr),
        other.data_ptr,
    }) {}
    struct virtual_type;
    template<typename Impl>
    struct methods_impl;
    template<typename Impl>
    static const string_op_PlayString_ftable0 ftbl_impl;
    template<typename Impl>
    static constexpr string_op_PlayString_vtable0 vtbl_impl = {
        .ftbl_ptr_0 = &::string_op::weak::PlayString::template ftbl_impl<Impl>,
    };
    template<typename Impl>
    static constexpr struct IdMapItem idmap_impl[1] = {
        {&string_op_PlayString_i0, &vtbl_impl<Impl>.ftbl_ptr_0},
    };
    using vtable_type = string_op_PlayString_vtable0;
    using view_type = ::string_op::weak::PlayString;
    using holder_type = ::string_op::PlayString;
    using abi_type = string_op_PlayString_t0;
    bool is_error() const& {
        return m_handle.vtbl_ptr == nullptr;
    }
    virtual_type const& operator*() const& {
        return *reinterpret_cast<virtual_type const*>(&m_handle);
    }
    virtual_type const* operator->() const& {
        return reinterpret_cast<virtual_type const*>(&m_handle);
    }
};
}
namespace string_op {
struct PlayString : public ::string_op::weak::PlayString {
    static constexpr bool is_holder = true;
    explicit PlayString(struct string_op_PlayString_t0 handle) : ::string_op::weak::PlayString(handle) {}
    PlayString& operator=(::string_op::PlayString other) {
        ::std::swap(this->m_handle, other.m_handle);
        return *this;
    }
    ~PlayString() {
        tobj_drop(this->m_handle.data_ptr);
    }
    operator ::taihe::data_view() const& {
        return ::taihe::data_view(this->m_handle.data_ptr);
    }
    operator ::taihe::data_holder() const& {
        return ::taihe::data_holder(tobj_dup(this->m_handle.data_ptr));
    }
    operator ::taihe::data_holder() && {
        return ::taihe::data_holder(std::exchange(this->m_handle.data_ptr, nullptr));
    }
    explicit PlayString(::taihe::data_holder other) : PlayString({
        string_op_PlayString_dynamic0(other.data_ptr->rtti_ptr),
        std::exchange(other.data_ptr, nullptr),
    }) {}
    PlayString(::string_op::weak::PlayString const& other) : PlayString({
        other.m_handle.vtbl_ptr,
        tobj_dup(other.m_handle.data_ptr),
    }) {}
    PlayString(::string_op::PlayString const& other) : PlayString({
        other.m_handle.vtbl_ptr,
        tobj_dup(other.m_handle.data_ptr),
    }) {}
    PlayString(::string_op::PlayString&& other) : PlayString({
        other.m_handle.vtbl_ptr,
        std::exchange(other.m_handle.data_ptr, nullptr),
    }) {}
};
}
namespace string_op::weak {
inline bool operator==(::string_op::weak::PlayString lhs, ::string_op::weak::PlayString rhs) {
    return ::taihe::data_view(lhs) == ::taihe::data_view(rhs);
}
}
template<> struct ::std::hash<::string_op::PlayString> {
    size_t operator()(::string_op::weak::PlayString val) const {
        return ::std::hash<::taihe::data_holder>()(val);
    }
};
#pragma clang diagnostic pop
