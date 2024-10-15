#pragma once
#include <taihe/common.h>
TH_EXPORT int32_t __integer__arithmetic__add_i32(int32_t a, int32_t b);
TH_EXPORT int32_t __integer__arithmetic__sub_i32(int32_t a, int32_t b);
TH_EXPORT int32_t __integer__arithmetic__mul_i32(int32_t a, int32_t b);
struct __integer__arithmetic__divmod_i32__return_t {
    int32_t _0;
    int32_t _1;
};
TH_EXPORT struct __integer__arithmetic__divmod_i32__return_t __integer__arithmetic__divmod_i32(int32_t a, int32_t b);
