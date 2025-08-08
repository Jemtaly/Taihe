#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe.platform.ani.AniObject.abi.0.h"
struct taihe_platform_ani_AniObject_ftable;
struct taihe_platform_ani_AniObject_vtable {
    struct taihe_platform_ani_AniObject_ftable const* ftbl_ptr_0;
};
TH_EXPORT void const* const taihe_platform_ani_AniObject_i;
struct taihe_platform_ani_AniObject_t {
    struct taihe_platform_ani_AniObject_vtable const* vtbl_ptr;
    struct DataBlockHead* data_ptr;
};
TH_INLINE struct taihe_platform_ani_AniObject_vtable const* taihe_platform_ani_AniObject_dynamic(struct TypeInfo const* rtti_ptr) {
    for (size_t i = 0; i < rtti_ptr->len; i++) {
        if (rtti_ptr->idmap[i].id == taihe_platform_ani_AniObject_i) {
            return (struct taihe_platform_ani_AniObject_vtable const*)rtti_ptr->idmap[i].vtbl_ptr;
        }
    }
    return NULL;
}
#pragma clang diagnostic pop
