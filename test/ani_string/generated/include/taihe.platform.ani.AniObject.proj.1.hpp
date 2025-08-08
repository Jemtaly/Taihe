#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe.platform.ani.AniObject.proj.0.hpp"
#include "taihe.platform.ani.AniObject.abi.1.h"
namespace taihe::platform::ani::weak {
struct AniObject {
    static constexpr bool is_holder = false;
    struct taihe_platform_ani_AniObject_t m_handle;
    explicit AniObject(struct taihe_platform_ani_AniObject_t handle) : m_handle(handle) {}
    operator ::taihe::data_view() const& {
        return ::taihe::data_view(this->m_handle.data_ptr);
    }
    operator ::taihe::data_holder() const& {
        return ::taihe::data_holder(tobj_dup(this->m_handle.data_ptr));
    }
    explicit AniObject(::taihe::data_view other) : AniObject({
        taihe_platform_ani_AniObject_dynamic(other.data_ptr->rtti_ptr),
        other.data_ptr,
    }) {}
    struct virtual_type;
    template<typename Impl>
    struct methods_impl;
    template<typename Impl>
    static const taihe_platform_ani_AniObject_ftable ftbl_impl;
    template<typename Impl>
    static constexpr taihe_platform_ani_AniObject_vtable vtbl_impl = {
        .ftbl_ptr_0 = &::taihe::platform::ani::weak::AniObject::template ftbl_impl<Impl>,
    };
    template<typename Impl>
    static constexpr struct IdMapItem idmap_impl[1] = {
        {&taihe_platform_ani_AniObject_i, &vtbl_impl<Impl>.ftbl_ptr_0},
    };
    using vtable_type = taihe_platform_ani_AniObject_vtable;
    using view_type = ::taihe::platform::ani::weak::AniObject;
    using holder_type = ::taihe::platform::ani::AniObject;
    using abi_type = taihe_platform_ani_AniObject_t;
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
namespace taihe::platform::ani {
struct AniObject : public ::taihe::platform::ani::weak::AniObject {
    static constexpr bool is_holder = true;
    explicit AniObject(struct taihe_platform_ani_AniObject_t handle) : ::taihe::platform::ani::weak::AniObject(handle) {}
    AniObject& operator=(::taihe::platform::ani::AniObject other) {
        ::std::swap(this->m_handle, other.m_handle);
        return *this;
    }
    ~AniObject() {
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
    explicit AniObject(::taihe::data_holder other) : AniObject({
        taihe_platform_ani_AniObject_dynamic(other.data_ptr->rtti_ptr),
        std::exchange(other.data_ptr, nullptr),
    }) {}
    AniObject(::taihe::platform::ani::weak::AniObject const& other) : AniObject({
        other.m_handle.vtbl_ptr,
        tobj_dup(other.m_handle.data_ptr),
    }) {}
    AniObject(::taihe::platform::ani::AniObject const& other) : AniObject({
        other.m_handle.vtbl_ptr,
        tobj_dup(other.m_handle.data_ptr),
    }) {}
    AniObject(::taihe::platform::ani::AniObject&& other) : AniObject({
        other.m_handle.vtbl_ptr,
        std::exchange(other.m_handle.data_ptr, nullptr),
    }) {}
};
}
namespace taihe::platform::ani::weak {
inline bool operator==(::taihe::platform::ani::weak::AniObject lhs, ::taihe::platform::ani::weak::AniObject rhs) {
    return ::taihe::data_view(lhs) == ::taihe::data_view(rhs);
}
}
template<> struct ::std::hash<::taihe::platform::ani::AniObject> {
    size_t operator()(::taihe::platform::ani::weak::AniObject val) const {
        return ::std::hash<::taihe::data_holder>()(val);
    }
};
#pragma clang diagnostic pop
