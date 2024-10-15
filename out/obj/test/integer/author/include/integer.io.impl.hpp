#pragma once
#include <taihe/common.hpp>
#include <integer.io.abi.h>
#define TH_EXPORT_CPP_API_input_i32(_func) \
    TH_STATIC_ASSERT(TH_IS_SAME(TH_TYPEOF(_func), int32_t ()), \
        "'" #_func "' is incompatible with 'int32_t ::integer::io::input_i32()'"); \
    int32_t __integer__io__input_i32() { \
        return taihe::core::into_abi<int32_t, int32_t>(_func()); \
    }
#define TH_EXPORT_CPP_API_output_i32(_func) \
    TH_STATIC_ASSERT(TH_IS_SAME(TH_TYPEOF(_func), void (int32_t n)), \
        "'" #_func "' is incompatible with 'void ::integer::io::output_i32(int32_t n)'"); \
    void __integer__io__output_i32(int32_t n) { \
        return (_func(taihe::core::from_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(n)))); \
    }
