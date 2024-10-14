#pragma once
#include <taihe/common.h>
#include <integer.arithmetic.abi.h>
#define TH_EXPORT_C_API_add_i32(_func) \
    TH_STATIC_ASSERT(TH_IS_SAME(TH_TYPEOF(_func), int32_t (int32_t a, int32_t b)), \
        "'" #_func "' is incompatible with 'int32_t __integer__arithmetic__add_i32(int32_t a, int32_t b)'"); \
    int32_t __integer__arithmetic__add_i32(int32_t a, int32_t b) { \
        return _func(a, b); \
    }
#define TH_EXPORT_C_API_sub_i32(_func) \
    TH_STATIC_ASSERT(TH_IS_SAME(TH_TYPEOF(_func), int32_t (int32_t a, int32_t b)), \
        "'" #_func "' is incompatible with 'int32_t __integer__arithmetic__sub_i32(int32_t a, int32_t b)'"); \
    int32_t __integer__arithmetic__sub_i32(int32_t a, int32_t b) { \
        return _func(a, b); \
    }
#define TH_EXPORT_C_API_mul_i32(_func) \
    TH_STATIC_ASSERT(TH_IS_SAME(TH_TYPEOF(_func), int32_t (int32_t a, int32_t b)), \
        "'" #_func "' is incompatible with 'int32_t __integer__arithmetic__mul_i32(int32_t a, int32_t b)'"); \
    int32_t __integer__arithmetic__mul_i32(int32_t a, int32_t b) { \
        return _func(a, b); \
    }
#define TH_EXPORT_C_API_divmod_i32(_func) \
    TH_STATIC_ASSERT(TH_IS_SAME(TH_TYPEOF(_func), struct __integer__arithmetic__divmod_i32__return_t (int32_t a, int32_t b)), \
        "'" #_func "' is incompatible with 'struct __integer__arithmetic__divmod_i32__return_t __integer__arithmetic__divmod_i32(int32_t a, int32_t b)'"); \
    struct __integer__arithmetic__divmod_i32__return_t __integer__arithmetic__divmod_i32(int32_t a, int32_t b) { \
        return _func(a, b); \
    }
