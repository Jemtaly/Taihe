#include "sys.time.proj.hpp"
#include "core/promise.hpp"

#include <iostream>
#include <thread>
#include <chrono>

using namespace sys::time;
using namespace taihe::core;

template<typename T>
auto make_future_value(T&& value, uint64_t ms) {
    return make_promise<T, string>(
        [value = std::forward<T>(value), ms](promise_view<T, string> p) {
            setTimeout(into_holder<ICallback>([p = promise_holder<T, string>(p), value, ms]() { p->resolve(value); }), ms);
        }
    );
}

int main() {
    std::cout << "Before promise" << std::endl;
    auto a = make_future_value(42, 1000)
        ->success_then(
            [](int val) {
                std::cout << "Got value: " << val << std::endl;
                std::cout << "Please input in 3 sec:" << std::endl;
                return make_promise<string, string, IPromiseStringString>(getInputWithTimeout, 3);
            }
        )
        ->add_success_callback(
            [](string_view sv) {
                std::cout << "Your input: " << sv.c_str() << std::endl;
            }
        )->add_error_callback(
            [](string_view sv) {
                std::cout << "Error: " << sv.c_str() << std::endl;
            }
        );
    std::cout << "After promise" << std::endl;
    a->wait();
    std::cout << "Done" << std::endl;
}
