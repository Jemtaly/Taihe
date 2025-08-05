#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe/common.hpp"
#include "hello.abi.h"
#define TH_EXPORT_CPP_API_sayHello(CPP_FUNC_IMPL) \
    void hello_sayHello_f() { \
        return CPP_FUNC_IMPL(); \
    }
#pragma clang diagnostic pop
