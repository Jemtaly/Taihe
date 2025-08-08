#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe.platform.ani.AniObject.abi.1.h"
struct taihe_platform_ani_AniObject_ftable {
    uintptr_t (*getGlobalReference)(struct taihe_platform_ani_AniObject_t tobj);
};
TH_INLINE uintptr_t taihe_platform_ani_AniObject_getGlobalReference_f(struct taihe_platform_ani_AniObject_t tobj) {
    return tobj.vtbl_ptr->ftbl_ptr_0->getGlobalReference(tobj);
}
#pragma clang diagnostic pop
