#if defined(__OHOS__)
#include <napi/native_api.h>
#else
#include <node/node_api.h>
#endif

#include "impl.h"

namespace api_napi {

napi_value wrap_plus(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value argv[2];
  napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

  int a, b;
  napi_get_value_int32(env, argv[0], &a);
  napi_get_value_int32(env, argv[1], &b);

  int result = api_impl::plus(a, b);
  napi_value return_value;
  napi_create_int32(env, result, &return_value);
  return return_value;
}

napi_value wrap_concat(napi_env env, napi_callback_info info) {
  napi_status status;

  size_t argc = 1;
  napi_value argv[1];
  status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Failed to parse arguments");
    return nullptr;
  }

  if (argc != 1) {
    napi_throw_error(env, nullptr, "Incorrect number of arguments");
    return nullptr;
  }

  // Check if the argument is an array
  bool is_array;
  status = napi_is_array(env, argv[0], &is_array);
  if (status != napi_ok || !is_array) {
    napi_throw_error(env, nullptr, "Argument must be an array");
    return nullptr;
  }

  // Get the length of the array
  uint32_t length;
  status = napi_get_array_length(env, argv[0], &length);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Failed to get array length");
    return nullptr;
  }

  // Convert the NAPI array to a std::vector<std::string>
  std::vector<std::string> strings(length);
  for (uint32_t i = 0; i < length; i++) {
    napi_value element;
    status = napi_get_element(env, argv[0], i, &element);
    if (status != napi_ok) {
      napi_throw_error(env, nullptr, "Failed to get array element");
      return nullptr;
    }

    size_t utf8_size;
    status = napi_get_value_string_utf8(env, element, nullptr, 0, &utf8_size);
    if (status != napi_ok) {
      napi_throw_error(env, nullptr, "Failed to get string length");
      return nullptr;
    }

    char *buffer = new char[utf8_size + 1];
    status = napi_get_value_string_utf8(env, element, buffer, utf8_size + 1,
                                        &utf8_size);
    if (status != napi_ok) {
      delete[] buffer;
      napi_throw_error(env, nullptr, "Failed to get string value");
      return nullptr;
    }
    buffer[utf8_size] = '\0';
    strings[i] = buffer;
    delete[] buffer;
  }

  // Call the C++ function
  std::string result = api_impl::concat(strings);

  // Convert the result to a NAPI string.
  napi_value return_value;
  status = napi_create_string_utf8(env, result.c_str(), result.length(),
                                   &return_value);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Failed to create return value");
    return nullptr;
  }

  return return_value;
}

napi_value module_init(napi_env env, napi_value exports) {
  napi_property_descriptor desc[] = {
      {"plus", nullptr, wrap_plus, nullptr, nullptr, nullptr, napi_default,
       nullptr},
      {"concat", nullptr, wrap_concat, nullptr, nullptr, nullptr, napi_default,
       nullptr},
  };

  napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
  return exports;
}

}  // namespace api_napi

NAPI_MODULE(my_api, api_napi::module_init);
