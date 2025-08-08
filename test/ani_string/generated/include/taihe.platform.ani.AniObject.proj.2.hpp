#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe.platform.ani.AniObject.proj.1.hpp"
#include "taihe.platform.ani.AniObject.abi.2.h"
struct ::taihe::platform::ani::weak::AniObject::virtual_type {
    uintptr_t getGlobalReference() const& {
        return ::taihe::from_abi<uintptr_t>(taihe_platform_ani_AniObject_getGlobalReference_f(*reinterpret_cast<taihe_platform_ani_AniObject_t const*>(this)));
    }
};
template<typename Impl>
struct ::taihe::platform::ani::weak::AniObject::methods_impl {
    static uintptr_t getGlobalReference(struct taihe_platform_ani_AniObject_t tobj) {
        return ::taihe::into_abi<uintptr_t>(::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->getGlobalReference());
    }
};
template<typename Impl>
constexpr taihe_platform_ani_AniObject_ftable taihe::platform::ani::weak::AniObject::ftbl_impl = {
    .getGlobalReference = &methods_impl<Impl>::getGlobalReference,
};
#pragma clang diagnostic pop
