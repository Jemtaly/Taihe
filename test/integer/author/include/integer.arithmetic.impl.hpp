#pragma once
#include <taihe/common.hpp>
#include <integer.arithmetic.abi.h>
#define TH_EXPORT_CPP_API_add_i32(_func) \
    TH_STATIC_ASSERT(TH_IS_SAME(TH_TYPEOF(_func), int32_t (int32_t a, int32_t b)), \
        "'" #_func "' is incompatible with 'int32_t ::integer::arithmetic::add_i32(int32_t a, int32_t b)'"); \
    int32_t __integer__arithmetic__add_i32(int32_t a, int32_t b) { \
        return taihe::core::into_abi<int32_t, int32_t>(_func(taihe::core::from_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(a)), taihe::core::from_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(b)))); \
    }
#define TH_EXPORT_CPP_API_sub_i32(_func) \
    TH_STATIC_ASSERT(TH_IS_SAME(TH_TYPEOF(_func), int32_t (int32_t a, int32_t b)), \
        "'" #_func "' is incompatible with 'int32_t ::integer::arithmetic::sub_i32(int32_t a, int32_t b)'"); \
    int32_t __integer__arithmetic__sub_i32(int32_t a, int32_t b) { \
        return taihe::core::into_abi<int32_t, int32_t>(_func(taihe::core::from_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(a)), taihe::core::from_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(b)))); \
    }
#define TH_EXPORT_CPP_API_mul_i32(_func) \
    TH_STATIC_ASSERT(TH_IS_SAME(TH_TYPEOF(_func), int32_t (int32_t a, int32_t b)), \
        "'" #_func "' is incompatible with 'int32_t ::integer::arithmetic::mul_i32(int32_t a, int32_t b)'"); \
    int32_t __integer__arithmetic__mul_i32(int32_t a, int32_t b) { \
        return taihe::core::into_abi<int32_t, int32_t>(_func(taihe::core::from_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(a)), taihe::core::from_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(b)))); \
    }
template<>
inline struct __integer__arithmetic__divmod_i32__return_t taihe::core::into_abi(std::add_rvalue_reference_t<std::tuple<int32_t, int32_t>> _val) {
    return {
        taihe::core::into_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(std::get<0>(_val))), 
        taihe::core::into_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(std::get<1>(_val))), 
    };
}
#define TH_EXPORT_CPP_API_divmod_i32(_func) \
    TH_STATIC_ASSERT(TH_IS_SAME(TH_TYPEOF(_func), std::tuple<int32_t, int32_t> (int32_t a, int32_t b)), \
        "'" #_func "' is incompatible with 'std::tuple<int32_t, int32_t> ::integer::arithmetic::divmod_i32(int32_t a, int32_t b)'"); \
    struct __integer__arithmetic__divmod_i32__return_t __integer__arithmetic__divmod_i32(int32_t a, int32_t b) { \
        return taihe::core::into_abi<std::tuple<int32_t, int32_t>, struct __integer__arithmetic__divmod_i32__return_t>(_func(taihe::core::from_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(a)), taihe::core::from_abi<int32_t, int32_t>(static_cast<std::add_rvalue_reference_t<int32_t>>(b)))); \
    }
