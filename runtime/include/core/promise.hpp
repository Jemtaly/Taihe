#pragma once

#include <functional>
#include <variant>
#include <iostream>
#include <vector>
#include <condition_variable>
#include <mutex>

#include <taihe/common.hpp>
#include <core/object.hpp>

namespace taihe::core {
template<typename T, typename E>
struct promise;

template<typename T, typename E>
using promise_holder = impl_holder<promise<T, E>>;

template<typename T, typename E>
using promise_view = impl_view<promise<T, E>>;

template<typename T, typename E, typename... InterfaceHolders>
auto make_promise() {
    auto result = make_holder<promise<T, E>, InterfaceHolders...>();
    return result;
}

template<typename T, typename E, typename... InterfaceHolders, typename AsyncFunc, typename... Args>
auto make_promise(AsyncFunc&& asyncFunc, Args&&... args) {
    auto result = make_holder<promise<T, E>, InterfaceHolders...>();
    asyncFunc(result, std::forward<Args>(args)...);
    return result;
}

template<typename T, typename E, typename... Args>
auto make_resolved(Args&&... args) {
    auto result = make_holder<promise<T, E>>();
    result->resolve(std::forward<Args>(args)...);
    return result;
}

template<typename T, typename E, typename... Args>
auto make_rejected(Args&&... args) {
    auto result = make_holder<promise<T, E>>();
    result->reject(std::forward<Args>(args)...);
    return result;
}

template<typename T, typename E>
struct promise {
    using result_type = T;
    using error_type = E;

private:
    std::vector<std::function<auto (T const& value) -> void>> successCallbacks;
    std::vector<std::function<auto (E const& value) -> void>> errorCallbacks;
    std::variant<std::monostate, T, E> optValue;

    std::mutex mutex;
    std::condition_variable cv;

public:
    promise() : optValue(std::in_place_index<0>) {}
    ~promise() {}

    // author
    template<typename... Args>
    void resolve(Args&&... args) {
        T const* ptrValue;
        {
            std::unique_lock<std::mutex> lock(mutex);
            if (optValue.index() != 0) {
                return;
            }
            ptrValue = &optValue.template emplace<1>(std::forward<Args>(args)...);
        }
        cv.notify_all();
        for (const auto& callback : successCallbacks) {
            callback(*ptrValue);
        }
        successCallbacks.clear();
        errorCallbacks.clear();
    }

    template<typename... Args>
    void reject(Args&&... args) {
        E const* ptrError;
        {
            std::unique_lock<std::mutex> lock(mutex);
            if (optValue.index() != 0) {
                return;
            }
            ptrError = &optValue.template emplace<2>(std::forward<Args>(args)...);
        }
        cv.notify_all();
        for (const auto& callback : errorCallbacks) {
            callback(*ptrError);
        }
        successCallbacks.clear();
        errorCallbacks.clear();
    }

    // user
    void wait() {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock, [this]() { return optValue.index() != 0; });
    }

    template<typename Callback>
    auto* add_success_callback(Callback&& callback) {
        if (optValue.index() == 0) {
            successCallbacks.push_back(callback);
        } else if (T const* ptrValue = std::get_if<1>(&optValue)) {
            callback(*ptrValue);
        }
        return this;
    }

    template<typename Callback>
    auto* add_error_callback(Callback&& callback) {
        if (optValue.index() == 0) {
            errorCallbacks.push_back(callback);
        } else if (E const* ptrValue = std::get_if<2>(&optValue)) {
            callback(*ptrValue);
        }
        return this;
    }

    template<typename Callback>
    auto success_then(Callback&& callback) {
        using promise_type = std::remove_reference_t<decltype(*callback(std::declval<T>()))>;
        using U = typename promise_type::result_type;
        auto next = make_promise<U, E>();
        add_success_callback([callback, next](T const& value) {
            auto result = callback(value);
            result->add_success_callback([next](U const& val) {
                next->resolve(val);
            });
            result->add_error_callback([next](E const& err) {
                next->reject(err);
            });
        });
        add_error_callback([next](E const& error) {
            next->reject(error);
        });
        return next;
    }

    template<typename Callback>
    auto error_then(Callback&& callback) {
        using promise_type = std::remove_reference_t<decltype(*callback(std::declval<E>()))>;
        using U = typename promise_type::result_type;
        using F = typename promise_type::error_type;
        auto next = make_promise<U, F>();
        add_error_callback([callback, next](E const& error) {
            auto result = callback(error);
            result->add_success_callback([next](U const& val) {
                next->resolve(val);
            });
            result->add_error_callback([next](F const& err) {
                next->reject(err);
            });
        });
        return next;
    }
};
}
