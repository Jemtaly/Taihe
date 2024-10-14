#pragma once
#include <taihe/common.h>
#include <integer.io.abi.h>
#define TH_EXPORT_C_API_input_i32(_func) \
    TH_STATIC_ASSERT(TH_IS_SAME(TH_TYPEOF(_func), int32_t ()), \
        "'" #_func "' is incompatible with 'int32_t __integer__io__input_i32()'"); \
    int32_t __integer__io__input_i32() { \
        return _func(); \
    }
#define TH_EXPORT_C_API_output_i32(_func) \
    TH_STATIC_ASSERT(TH_IS_SAME(TH_TYPEOF(_func), void (int32_t n)), \
        "'" #_func "' is incompatible with 'void __integer__io__output_i32(int32_t n)'"); \
    void __integer__io__output_i32(int32_t n) { \
        return _func(n); \
    }
