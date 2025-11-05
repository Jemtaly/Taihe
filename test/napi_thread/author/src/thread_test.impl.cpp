#include "thread_test.impl.hpp"

#include <chrono>
#include <iostream>
#include <thread>

using namespace taihe;

namespace {
static constexpr int32_t THOUSAND = 1000;

void invokeFromOtherThreadAfter(double sec,
                                ::taihe::callback_view<int32_t(int32_t a)> cb) {
  // Directly call
  cb(1);

  // call in the new thread
  std::cout << "!!!!!!!!!!" << std::endl;
  std::cerr << "-- begin invokeFromOtherThreadAfter --" << std::endl;
  std::thread thread([sec, cb = callback<int32_t(int32_t a)>(cb)]() {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(static_cast<int>(sec * THOUSAND)));
    std::cerr << "invokeFromOtherThreadAfter: " << sec << " seconds"
              << std::endl;
    std::cout << "result: " << cb(1) << std::endl;
  });
  thread.detach();
  std::cerr << "-- end invokeFromOtherThreadAfter --" << std::endl;
}
}  // namespace

// NOLINTBEGIN
TH_EXPORT_CPP_API_invokeFromOtherThreadAfter(invokeFromOtherThreadAfter);
// NOLINTEND
