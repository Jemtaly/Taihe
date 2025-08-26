#include "hello.impl.hpp"
#include "hello.proj.hpp"
#include "stdexcept"

#include <chrono>
#include <iostream>
#include <thread>

namespace {
void futureResultWithCallback(int64_t ms, taihe::string_view val,
                              taihe::async_setter<taihe::string> setter) {
  std::thread([ms, val = taihe::string(val),
               setter = std::move(setter)]() mutable {
    std::cout << "[Future Result] Waiting for " << ms << " milliseconds..."
              << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    std::cout << "[Future Result] Task completed, setting result..."
              << std::endl;
    setter.emplace_result(std::move(val));
  }).detach();
}

taihe::async_result<taihe::string> futureResultReturnsPromise(
    int64_t ms, taihe::string_view val) {
  auto [setter, result] = taihe::make_async_pair<taihe::string>();
  futureResultWithCallback(ms, val, std::move(setter));
  return std::move(result);
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_futureResultWithCallback(futureResultWithCallback);
TH_EXPORT_CPP_API_futureResultReturnsPromise(futureResultReturnsPromise);
// NOLINTEND
