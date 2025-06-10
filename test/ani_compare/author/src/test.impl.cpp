#include <vector>
#include "ani.h"
#include "taihe/array.hpp"
#include "taihe/runtime.hpp"
#include "taihe/string.hpp"

#include "taihe.platform.ani.proj.hpp"
#include "test.impl.hpp"
#include "test.proj.hpp"

namespace {
// To be implemented.

bool ani_ref_equals(::taihe::data_view lhs, ::taihe::data_view rhs) {
  auto lhs_as_ani = taihe::platform::ani::weak::AniObject(lhs);
  auto rhs_as_ani = taihe::platform::ani::weak::AniObject(rhs);
  if (lhs_as_ani.is_error() || rhs_as_ani.is_error()) {
    return false;
  }
  ani_env *env = taihe::get_env();
  ani_ref lhs_ref = reinterpret_cast<ani_ref>(lhs_as_ani->getGlobalReference());
  ani_ref rhs_ref = reinterpret_cast<ani_ref>(rhs_as_ani->getGlobalReference());
  ani_boolean result;
  return env->Reference_Equals(lhs_ref, rhs_ref, &result) == ANI_OK && result;
}

class CallbackManagerImpl {
  std::vector<::taihe::callback<taihe::string()>> callbacks_;

public:
  CallbackManagerImpl() {}

  bool addCallback(::taihe::callback_view<taihe::string()> new_cb) {
    for (auto const &old_cb : callbacks_) {
      if (ani_ref_equals(old_cb, new_cb)) {
        std::cerr << "Callback already exists." << std::endl;
        return false;
      }
    }
    callbacks_.emplace_back(new_cb);
    return true;
  }

  bool removeCallback(::taihe::callback_view<taihe::string()> cb) {
    for (auto it = callbacks_.begin(); it != callbacks_.end(); ++it) {
      if (ani_ref_equals(*it, cb)) {
        callbacks_.erase(it);
        return true;
      }
    }
    std::cerr << "Callback not found." << std::endl;
    return false;
  }

  taihe::array<taihe::string> invokeCallbacks() {
    std::vector<taihe::string> results;
    for (auto const &cb : callbacks_) {
      results.push_back(cb());
    }
    return taihe::array<taihe::string>(taihe::copy_data, results.data(),
                                       results.size());
  }
};

::test::CallbackManager getCallbackManager() {
  return taihe::make_holder<CallbackManagerImpl, ::test::CallbackManager>();
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_getCallbackManager(getCallbackManager);
// NOLINTEND
