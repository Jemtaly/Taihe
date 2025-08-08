#include "string_op.proj.hpp"
#include "string_op.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"


namespace {
// To be implemented.

class PlayStringImpl {
    public:
    PlayStringImpl() {
        // Don't forget to implement the constructor.
    }

    ::taihe::string pickString(::taihe::array_view<::taihe::string> nums, int32_t n1, int32_t n2) {
        TH_THROW(std::runtime_error, "pickString not implemented");
    }

    ::taihe::string getName() {
        TH_THROW(std::runtime_error, "getName not implemented");
    }

    void setName(::taihe::string_view name) {
        TH_THROW(std::runtime_error, "setName not implemented");
    }
};

::taihe::string concatString(::taihe::string_view a, ::taihe::string_view b) {
    TH_THROW(std::runtime_error, "concatString not implemented");
}

::taihe::string makeString(::taihe::string_view a, int32_t b) {
    TH_THROW(std::runtime_error, "makeString not implemented");
}

::string_op::StringPair split(::taihe::string_view a, int32_t n) {
    TH_THROW(std::runtime_error, "split not implemented");
}

::taihe::array<::taihe::string> split2(::taihe::string_view a, int32_t n) {
    TH_THROW(std::runtime_error, "split2 not implemented");
}

int32_t to_i32(::taihe::string_view a) {
    TH_THROW(std::runtime_error, "to_i32 not implemented");
}

::taihe::string from_i32(int32_t a) {
    TH_THROW(std::runtime_error, "from_i32 not implemented");
}

::string_op::PlayString makePlayStringIface() {
    // The parameters in the make_holder function should be of the same type
    // as the parameters in the constructor of the actual implementation class.
    return taihe::make_holder<PlayStringImpl, ::string_op::PlayString>();
}

float to_f32(::taihe::string_view a) {
    TH_THROW(std::runtime_error, "to_f32 not implemented");
}

::taihe::string from_f32(float a) {
    TH_THROW(std::runtime_error, "from_f32 not implemented");
}

::taihe::string concatString2(::taihe::string_view s, int32_t n, ::taihe::array_view<::taihe::string> sArr, bool b, ::taihe::array_view<uint8_t> buffer) {
    TH_THROW(std::runtime_error, "concatString2 not implemented");
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_concatString(concatString);
TH_EXPORT_CPP_API_makeString(makeString);
TH_EXPORT_CPP_API_split(split);
TH_EXPORT_CPP_API_split2(split2);
TH_EXPORT_CPP_API_to_i32(to_i32);
TH_EXPORT_CPP_API_from_i32(from_i32);
TH_EXPORT_CPP_API_makePlayStringIface(makePlayStringIface);
TH_EXPORT_CPP_API_to_f32(to_f32);
TH_EXPORT_CPP_API_from_f32(from_f32);
TH_EXPORT_CPP_API_concatString2(concatString2);
// NOLINTEND
