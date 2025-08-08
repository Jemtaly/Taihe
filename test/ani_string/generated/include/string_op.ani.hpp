#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe/platform/ani.hpp"
namespace string_op {
ani_status ANIRegister(ani_env *env);
}
#pragma clang diagnostic pop
