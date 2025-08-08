#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe/object.hpp"
#include "taihe.platform.ani.AniObject.abi.0.h"
namespace taihe::platform::ani::weak {
struct AniObject;
}
namespace taihe::platform::ani {
struct AniObject;
}
namespace taihe {
template<>
struct as_abi<::taihe::platform::ani::AniObject> {
    using type = struct taihe_platform_ani_AniObject_t;
};
template<>
struct as_abi<::taihe::platform::ani::weak::AniObject> {
    using type = struct taihe_platform_ani_AniObject_t;
};
template<>
struct as_param<::taihe::platform::ani::AniObject> {
    using type = ::taihe::platform::ani::weak::AniObject;
};
}
#pragma clang diagnostic pop
