#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe/common.hpp"
#include "string_op.abi.h"
#include "taihe/string.hpp"
#include "string_op.StringPair.proj.2.hpp"
#include "taihe/array.hpp"
#include "string_op.PlayString.proj.2.hpp"
#define TH_EXPORT_CPP_API_concatString(CPP_FUNC_IMPL) \
    struct TString string_op_concatString_f0(struct TString a, struct TString b) { \
        return ::taihe::into_abi<::taihe::string>(CPP_FUNC_IMPL(::taihe::from_abi<::taihe::string_view>(a), ::taihe::from_abi<::taihe::string_view>(b))); \
    }
#define TH_EXPORT_CPP_API_makeString(CPP_FUNC_IMPL) \
    struct TString string_op_makeString_f0(struct TString a, int32_t b) { \
        return ::taihe::into_abi<::taihe::string>(CPP_FUNC_IMPL(::taihe::from_abi<::taihe::string_view>(a), ::taihe::from_abi<int32_t>(b))); \
    }
#define TH_EXPORT_CPP_API_split(CPP_FUNC_IMPL) \
    struct string_op_StringPair_t0 string_op_split_f0(struct TString a, int32_t n) { \
        return ::taihe::into_abi<::string_op::StringPair>(CPP_FUNC_IMPL(::taihe::from_abi<::taihe::string_view>(a), ::taihe::from_abi<int32_t>(n))); \
    }
#define TH_EXPORT_CPP_API_split2(CPP_FUNC_IMPL) \
    struct TArray string_op_split2_f0(struct TString a, int32_t n) { \
        return ::taihe::into_abi<::taihe::array<::taihe::string>>(CPP_FUNC_IMPL(::taihe::from_abi<::taihe::string_view>(a), ::taihe::from_abi<int32_t>(n))); \
    }
#define TH_EXPORT_CPP_API_to_i32(CPP_FUNC_IMPL) \
    int32_t string_op_to_i32_f02(struct TString a) { \
        return ::taihe::into_abi<int32_t>(CPP_FUNC_IMPL(::taihe::from_abi<::taihe::string_view>(a))); \
    }
#define TH_EXPORT_CPP_API_from_i32(CPP_FUNC_IMPL) \
    struct TString string_op_from_i32_f02(int32_t a) { \
        return ::taihe::into_abi<::taihe::string>(CPP_FUNC_IMPL(::taihe::from_abi<int32_t>(a))); \
    }
#define TH_EXPORT_CPP_API_makePlayStringIface(CPP_FUNC_IMPL) \
    struct string_op_PlayString_t0 string_op_makePlayStringIface_f0() { \
        return ::taihe::into_abi<::string_op::PlayString>(CPP_FUNC_IMPL()); \
    }
#define TH_EXPORT_CPP_API_to_f32(CPP_FUNC_IMPL) \
    float string_op_to_f32_f02(struct TString a) { \
        return ::taihe::into_abi<float>(CPP_FUNC_IMPL(::taihe::from_abi<::taihe::string_view>(a))); \
    }
#define TH_EXPORT_CPP_API_from_f32(CPP_FUNC_IMPL) \
    struct TString string_op_from_f32_f02(float a) { \
        return ::taihe::into_abi<::taihe::string>(CPP_FUNC_IMPL(::taihe::from_abi<float>(a))); \
    }
#define TH_EXPORT_CPP_API_concatString2(CPP_FUNC_IMPL) \
    struct TString string_op_concatString2_f0(struct TString s, int32_t n, struct TArray sArr, bool b, struct TArray buffer) { \
        return ::taihe::into_abi<::taihe::string>(CPP_FUNC_IMPL(::taihe::from_abi<::taihe::string_view>(s), ::taihe::from_abi<int32_t>(n), ::taihe::from_abi<::taihe::array_view<::taihe::string>>(sArr), ::taihe::from_abi<bool>(b), ::taihe::from_abi<::taihe::array_view<uint8_t>>(buffer))); \
    }
#pragma clang diagnostic pop
