#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "string_op.StringPair.proj.0.hpp"
#include "string_op.StringPair.abi.1.h"
#include "taihe/string.hpp"
namespace string_op {
struct StringPair {
    ::taihe::string _0;
    ::taihe::string _1;
};
}
namespace string_op {
inline bool operator==(::string_op::StringPair const& lhs, ::string_op::StringPair const& rhs) {
    return true && lhs._0 == rhs._0 && lhs._1 == rhs._1;
}
}
template<> struct ::std::hash<::string_op::StringPair> {
    size_t operator()(::string_op::StringPair const& val) const {
        ::std::size_t seed = 0;
        seed ^= ::std::hash<::taihe::string>()(val._0) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= ::std::hash<::taihe::string>()(val._1) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};
#pragma clang diagnostic pop
