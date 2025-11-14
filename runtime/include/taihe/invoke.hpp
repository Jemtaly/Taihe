#pragma once

#include <taihe/common.hpp>
#include <taihe/expected.hpp>

namespace taihe {
template<typename OutT, typename cpp_return_t, typename... cpp_param_t,
         typename abi_func_t,
         std::enable_if_t<std::is_same_v<void, cpp_return_t>, int> = 0>
inline ::taihe::expected<cpp_return_t, ::taihe::error> call_abi_func(
    abi_func_t &&abi_func, cpp_param_t... params) {
  OutT _taihe_out;
  int32_t res_flag = abi_func(&_taihe_out, into_abi<cpp_param_t>(params)...);
  if (res_flag == 0) {
    return {};
  } else {
    return ::taihe::unexpected<::taihe::error>(
        ::taihe::from_abi<::taihe::error>(_taihe_out.error));
  }
}

template<typename OutT, typename cpp_return_t, typename... cpp_param_t,
         typename abi_func_t,
         std::enable_if_t<!std::is_same_v<void, cpp_return_t>, int> = 0>
inline ::taihe::expected<cpp_return_t, ::taihe::error> call_abi_func(
    abi_func_t &&abi_func, cpp_param_t... params) {
  OutT _taihe_out;
  int32_t res_flag = abi_func(&_taihe_out, into_abi<cpp_param_t>(params)...);
  if (res_flag == 0) {
    return from_abi<cpp_return_t>(_taihe_out.data);
  } else {
    return ::taihe::unexpected<::taihe::error>(
        ::taihe::from_abi<::taihe::error>(_taihe_out.error));
  }
}

template<typename OutT, typename cpp_return_t, typename... cpp_param_t,
         typename cpp_func_t,
         std::enable_if_t<std::is_same_v<void, cpp_return_t>, int> = 0>
inline int32_t call_cpp_func(OutT *_taihe_out, cpp_func_t &&cpp_func,
                             as_abi_t<cpp_param_t>... params) {
  using result_type = decltype(cpp_func(from_abi<cpp_param_t>(params)...));
  if constexpr (is_expected_v<result_type>) {
    auto result = cpp_func(from_abi<cpp_param_t>(params)...);
    if (result.has_value()) {
      return 0;
    } else {
      _taihe_out->error = ::taihe::into_abi<::taihe::error>(result.error());
      return 1;
    }
  } else {
    cpp_func(from_abi<cpp_param_t>(params)...);
    return 0;
  }
}

template<typename OutT, typename cpp_return_t, typename... cpp_param_t,
         typename cpp_func_t,
         std::enable_if_t<!std::is_same_v<void, cpp_return_t>, int> = 0>
inline int32_t call_cpp_func(OutT *_taihe_out, cpp_func_t &&cpp_func,
                             as_abi_t<cpp_param_t>... params) {
  using result_type = decltype(cpp_func(from_abi<cpp_param_t>(params)...));
  if constexpr (is_expected_v<result_type>) {
    auto result = cpp_func(from_abi<cpp_param_t>(params)...);
    if (result.has_value()) {
      _taihe_out->data = ::taihe::into_abi<cpp_return_t>(result.value());
      return 0;
    } else {
      _taihe_out->error = ::taihe::into_abi<::taihe::error>(result.error());
      return 1;
    }
  } else {
    auto result = cpp_func(from_abi<cpp_param_t>(params)...);
    _taihe_out->data = ::taihe::into_abi<cpp_return_t>(result);
    return 0;
  }
}

template<typename OutT, typename cpp_return_t, typename... cpp_param_t,
         typename cpp_obj_t, typename cpp_method_t,
         std::enable_if_t<std::is_same_v<void, cpp_return_t>, int> = 0>
inline int32_t call_cpp_method(OutT *_taihe_out, cpp_method_t &&cpp_method,
                               cpp_obj_t &&cpp_obj,
                               as_abi_t<cpp_param_t>... params) {
  using result_type = decltype((std::forward<cpp_obj_t>(cpp_obj).*
                                cpp_method)(from_abi<cpp_param_t>(params)...));
  if constexpr (is_expected_v<result_type>) {
    auto result = (std::forward<cpp_obj_t>(cpp_obj).*
                   cpp_method)(from_abi<cpp_param_t>(params)...);
    if (result.has_value()) {
      return 0;
    } else {
      _taihe_out->error = ::taihe::into_abi<::taihe::error>(result.error());
      return 1;
    }
  } else {
    (std::forward<cpp_obj_t>(cpp_obj).*
     cpp_method)(from_abi<cpp_param_t>(params)...);
    return 0;
  }
}

template<typename OutT, typename cpp_return_t, typename... cpp_param_t,
         typename cpp_obj_t, typename cpp_method_t,
         std::enable_if_t<!std::is_same_v<void, cpp_return_t>, int> = 0>
inline int32_t call_cpp_method(OutT *_taihe_out, cpp_method_t &&cpp_method,
                               cpp_obj_t &&cpp_obj,
                               as_abi_t<cpp_param_t>... params) {
  using result_type = decltype((std::forward<cpp_obj_t>(cpp_obj).*
                                cpp_method)(from_abi<cpp_param_t>(params)...));
  if constexpr (is_expected_v<result_type>) {
    auto result = (std::forward<cpp_obj_t>(cpp_obj).*
                   cpp_method)(from_abi<cpp_param_t>(params)...);
    if (result.has_value()) {
      _taihe_out->data = ::taihe::into_abi<cpp_return_t>(result.value());
      return 0;
    } else {
      _taihe_out->error = ::taihe::into_abi<::taihe::error>(result.error());
      return 1;
    }
  } else {
    auto result = (std::forward<cpp_obj_t>(cpp_obj).*
                   cpp_method)(from_abi<cpp_param_t>(params)...);
    _taihe_out->data = ::taihe::into_abi<cpp_return_t>(result);
    return 0;
  }
}
}  // namespace taihe