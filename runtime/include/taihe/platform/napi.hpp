/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef TAIHE_PLATFORM_NAPI_HPP
#define TAIHE_PLATFORM_NAPI_HPP

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <pthread.h>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

#include <taihe/array.hpp>
#include <taihe/async.hpp>
#include <taihe/object.hpp>
#include <taihe/runtime_napi.hpp>

#include <taihe.platform.napi.proj.hpp>

namespace taihe {
// convert between napi types and taihe types

template<typename cpp_owner_t>
struct from_napi_t;

template<typename cpp_owner_t>
struct into_napi_t;

template<typename cpp_owner_t>
constexpr inline from_napi_t<cpp_owner_t> from_napi;

template<typename cpp_owner_t>
constexpr inline into_napi_t<cpp_owner_t> into_napi;
}  // namespace taihe

namespace taihe {
class ThreadContext {
public:
    static ThreadContext &get_instance()
    {
        static ThreadContext instance;
        return instance;
    }

    void init_main_thread_id()
    {
        static std::once_flag flag;
        std::call_once(flag, [this]() {
            main_thread_id_ = pthread_self();
            initialized_ = true;
        });
    }

    bool is_main_thread() const
    {
        if (!initialized_) {
            return false;
        }
        return pthread_equal(pthread_self(), main_thread_id_) != 0;
    }

    ThreadContext(ThreadContext const &) = delete;
    ThreadContext &operator=(ThreadContext const &) = delete;

private:
    ThreadContext() = default;
    pthread_t main_thread_id_;
    bool initialized_ = false;
};

inline bool _is_main_thread()
{
    return ThreadContext::get_instance().is_main_thread();
}

inline void _init_main_thread()
{
    ThreadContext::get_instance().init_main_thread_id();
}

struct napi_ref_guard {
private:
    struct threadsafe_call {
        virtual void invoke(napi_env env) = 0;
        virtual ~threadsafe_call() = default;
    };

    napi_env env_;
    napi_ref ref_;
    napi_threadsafe_function tsfn_;

    static void dispatch_threadsafe_call(napi_env env, [[maybe_unused]] napi_value js_cb,
                                         [[maybe_unused]] void *context, void *data)
    {
        auto *call = static_cast<threadsafe_call *>(data);
        call->invoke(env);
    }

    void call_threadsafe_function(threadsafe_call *call)
    {
        TH_NAPI_ASSUME_CALL(env_, napi_call_threadsafe_function(tsfn_, call, napi_tsfn_blocking));
    }

protected:
    template<typename Callable, typename... Args>
    auto sync_call(Args &&...args) -> std::invoke_result_t<Callable &&, napi_env, napi_ref>
    {
        using result_t = std::invoke_result_t<Callable &&, napi_env, napi_ref>;
        using stored_result_t = std::conditional_t<std::is_void_v<result_t>, std::monostate, result_t>;

        if (::taihe::_is_main_thread()) {
            Callable callable {std::forward<Args>(args)...};
            return callable(env_, ref_);
        }

        struct sync_call_data final : threadsafe_call {
            std::mutex mutex;
            std::condition_variable cv;
            napi_ref ref;
            Callable callable;
            std::optional<stored_result_t> result;

            explicit sync_call_data(napi_ref ref, Args &&...args) : ref(ref), callable {std::forward<Args>(args)...}
            {
            }

            void invoke(napi_env env) override
            {
                std::lock_guard<std::mutex> lock(this->mutex);
                if constexpr (std::is_void_v<result_t>) {
                    this->callable(env, this->ref);
                    this->result.emplace();
                } else {
                    this->result = this->callable(env, this->ref);
                }
                this->cv.notify_one();
            }
        };

        sync_call_data data(ref_, std::forward<Args>(args)...);
        call_threadsafe_function(&data);
        std::unique_lock<std::mutex> lock(data.mutex);
        data.cv.wait(lock, [&data] {
            return data.result.has_value();
        });

        if constexpr (std::is_void_v<result_t>) {
            return;
        } else {
            return std::move(data.result).value();
        }
    }

    template<typename Callable, typename... Args>
    auto async_call(Args &&...args) -> taihe::future<std::invoke_result_t<Callable &&, napi_env, napi_ref>>
    {
        using result_t = std::invoke_result_t<Callable &&, napi_env, napi_ref>;

        struct async_call_data final : threadsafe_call {
            napi_ref ref;
            Callable callable;
            taihe::completer<result_t> completer;

            explicit async_call_data(napi_ref ref, Args &&...args, taihe::completer<result_t> completer)
                : ref(ref), callable {std::forward<Args>(args)...}, completer(std::move(completer))
            {
            }

            void invoke(napi_env env) override
            {
                this->completer.complete(this->callable(env, this->ref));
                delete this;
            }
        };

        auto [completer, future] = taihe::make_async_pair<result_t>();
        auto *dptr = new async_call_data(ref_, std::forward<Args>(args)..., std::move(completer));
        call_threadsafe_function(dptr);

        return future;
    }

public:
    napi_ref_guard(napi_env env, napi_value callback) : env_(env)
    {
        TH_NAPI_ASSUME_CALL(env, napi_create_reference(env, callback, 1, &ref_));
        napi_value name;
        TH_NAPI_ASSUME_CALL(env, napi_create_string_utf8(env, "MyWorkResource", NAPI_AUTO_LENGTH, &name));
        TH_NAPI_ASSUME_CALL(env, napi_create_threadsafe_function(env, nullptr, nullptr, name, 0, 1, nullptr, nullptr,
                                                                 nullptr, dispatch_threadsafe_call, &tsfn_));
        TH_NAPI_ASSUME_CALL(env, napi_unref_threadsafe_function(env, tsfn_));
    }

    ~napi_ref_guard()
    {
        if (ref_) {
            struct async_call_lambda {
                taihe::expected<void, taihe::error> operator()(napi_env env, napi_ref ref)
                {
                    TH_NAPI_TRY_CALL(env, napi_delete_reference(env, ref));
                    return {};
                }
            };

            this->async_call<async_call_lambda>();
        }
        if (tsfn_) {
            TH_NAPI_ASSUME_CALL(env_, napi_release_threadsafe_function(tsfn_, napi_tsfn_release));
        }
    }

    napi_ref_guard(napi_ref_guard const &) = delete;
    napi_ref_guard &operator=(napi_ref_guard const &) = delete;
    napi_ref_guard(napi_ref_guard &&other) = delete;
    napi_ref_guard &operator=(napi_ref_guard &&other) = delete;

    uintptr_t getGlobalReference() const
    {
        return reinterpret_cast<uintptr_t>(ref_);
    }
};

inline bool _get_bigint_msb(uint64_t dig)
{
    return dig >> (sizeof(uint64_t) * 8 - 1) != 0;
}

inline bool _get_bigint_sign(taihe::array_view<uint64_t> num)
{
    return _get_bigint_msb(num[num.size() - 1]);
}

inline std::pair<bool, taihe::array<uint64_t>> _get_bigint_sign_and_abs(taihe::array_view<uint64_t> num)
{
    bool sign = _get_bigint_sign(num);
    taihe::array_builder<uint64_t> abs(num.size());
    if (sign) {
        bool borrow = false;
        for (uint64_t numv : num) {
            uint64_t absv = 0 - numv - borrow;
            abs.push_back(absv);
            borrow = borrow || (absv != 0);
        }
    } else {
        bool carry = false;
        for (uint64_t numv : num) {
            uint64_t absv = 0 + numv + carry;
            abs.push_back(absv);
            carry = carry && (absv == 0);
        }
    }
    while (abs.size() > 0 && abs[abs.size() - 1] == 0) {
        abs.pop_back();
    }
    return {sign, std::move(abs).finish()};
}

inline taihe::array<uint64_t> _build_num(bool sign, taihe::array_view<uint64_t> abs)
{
    taihe::array_builder<uint64_t> num(abs.size() + 1);
    if (sign) {
        bool borrow = false;
        for (uint64_t absv : abs) {
            uint64_t numv = 0 - absv - borrow;
            num.push_back(numv);
            borrow = borrow || (numv != 0);
        }
        num.push_back(0 - borrow);
    } else {
        bool carry = false;
        for (uint64_t absv : abs) {
            uint64_t numv = 0 + absv + carry;
            num.push_back(numv);
            carry = carry && (numv == 0);
        }
        num.push_back(0 + carry);
    }
    while (num.size() - 1 > 0 && ((num[num.size() - 1] + 0 == 0 && _get_bigint_msb(num[num.size() - 1 - 1]) == 0) ||
                                  (num[num.size() - 1] + 1 == 0 && _get_bigint_msb(num[num.size() - 1 - 1]) == 1))) {
        num.pop_back();
    }
    return std::move(num).finish();
}
}  // namespace taihe

#endif  // TAIHE_PLATFORM_NAPI_HPP
