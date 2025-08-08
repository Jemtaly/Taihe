#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe/common.hpp"
#include "string_io.abi.h"
#include "taihe/string.hpp"
#define TH_EXPORT_CPP_API_input(CPP_FUNC_IMPL) \
    struct TString string_io_input_f0() { \
        return ::taihe::into_abi<::taihe::string>(CPP_FUNC_IMPL()); \
    }
#define TH_EXPORT_CPP_API_print(CPP_FUNC_IMPL) \
    void string_io_print_f0(struct TString a) { \
        return CPP_FUNC_IMPL(::taihe::from_abi<::taihe::string_view>(a)); \
    }
#define TH_EXPORT_CPP_API_println(CPP_FUNC_IMPL) \
    void string_io_println_f0(struct TString a) { \
        return CPP_FUNC_IMPL(::taihe::from_abi<::taihe::string_view>(a)); \
    }
#pragma clang diagnostic pop
