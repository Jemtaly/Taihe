#include <vector>
#include "ani.h"
#include "taihe/array.hpp"
#include "taihe/runtime.hpp"

#include "taihe/string.hpp"
#include "test.impl.hpp"
#include "test.proj.hpp"
#include "thlib.lang.proj.hpp"

namespace {
// To be implemented.

class CallbackManagerImpl {
  std::vector<::taihe::callback<taihe::string()>> callbacks_;

public:
  CallbackManagerImpl() {}

  bool addCallback(::taihe::callback_view<taihe::string()> new_cb) {
    auto new_obj = thlib::lang::weak::AniObject(new_cb);
    if (new_obj.is_error()) {
      std::cerr << "Failed to cast callback to ani." << std::endl;
      return false;
    }
    ani_ref new_ref = reinterpret_cast<ani_ref>(new_obj->getGlobalReference());
    ani_env *env = taihe::get_env();
    for (auto const &old_cb : callbacks_) {
      auto old_obj = thlib::lang::weak::AniObject(old_cb);
      ani_ref old_ref =
          reinterpret_cast<ani_ref>(old_obj->getGlobalReference());
      ani_boolean is_equal;
      if (env->Reference_Equals(old_ref, new_ref, &is_equal) != ANI_OK) {
        std::cerr << "Failed to compare references." << std::endl;
        return false;
      }
      if (is_equal) {
        std::cerr << "Callback already exists." << std::endl;
        return false;
      }
    }
    callbacks_.emplace_back(new_cb);
    return true;
  }

  bool removeCallback(::taihe::callback_view<taihe::string()> cb) {
    auto obj = thlib::lang::weak::AniObject(cb);
    if (obj.is_error()) {
      std::cerr << "Failed to cast callback to ani." << std::endl;
      return false;
    }
    ani_ref ref = reinterpret_cast<ani_ref>(obj->getGlobalReference());
    ani_env *env = taihe::get_env();
    for (auto it = callbacks_.begin(); it != callbacks_.end(); ++it) {
      auto old_obj = thlib::lang::weak::AniObject(*it);
      ani_ref old_ref =
          reinterpret_cast<ani_ref>(old_obj->getGlobalReference());
      ani_boolean is_equal;
      if (env->Reference_Equals(old_ref, ref, &is_equal) != ANI_OK) {
        std::cerr << "Failed to compare references." << std::endl;
        return false;
      }
      if (is_equal) {
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
