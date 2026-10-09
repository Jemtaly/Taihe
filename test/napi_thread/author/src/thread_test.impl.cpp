/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
// This file is a test file.
// NOLINTBEGIN

#include "thread_test.impl.hpp"

#include <chrono>
#include <iostream>
#include <thread>
#include "taihe/expected.hpp"
#include "taihe/async.hpp"

using namespace taihe;

namespace {
using VoidResult = taihe::expected<void, taihe::error>;
using StringCallback = taihe::callback<taihe::future<VoidResult>(taihe::string_view a)>;
using StringCallbackView = taihe::callback_view<taihe::future<VoidResult>(taihe::string_view a)>;
using StringResult = taihe::expected<taihe::string, taihe::error>;
using StringFuture = taihe::expected<taihe::future<StringResult>, taihe::error>;
using StringResultFunc = taihe::callback<StringResult()>;
using StringResultFuncView = taihe::callback_view<StringResult()>;
using StringFutureFunc = taihe::callback<StringFuture()>;
using StringFutureFuncView = taihe::callback_view<StringFuture()>;
using namespace thread_test;

VoidResult InvokeBlockingFunctionFromCurrentThread(StringResultFuncView func, StringCallbackView callback)
{
    std::cout << "invokeBlockingFunctionFromCurrentThread called" << std::endl;
    auto res = func();
    if (!res.has_value()) {
        std::cout << "invokeBlockingFunctionFromCurrentThread error: " << res.error().message() << std::endl;
        callback("<ERROR>");
    } else {
        std::cout << "invokeBlockingFunctionFromCurrentThread value: " << *res << std::endl;
        callback(*res + "-ProcessedInCurrentThread");
    }
    return {};
}

VoidResult InvokeNonBlockingFunctionFromCurrentThread(StringFutureFuncView func, StringCallbackView callback)
{
    std::cout << "invokeNonBlockingFunctionFromCurrentThread called" << std::endl;
    auto fut = func();
    fut->on_complete([callback = StringCallback(callback)](StringResult res) {
        if (!res.has_value()) {
            std::cout << "invokeNonBlockingFunctionFromCurrentThread error: " << res.error().message() << std::endl;
            callback("<ERROR>");
        } else {
            std::cout << "invokeNonBlockingFunctionFromCurrentThread value: " << *res << std::endl;
            callback(*res + "-ProcessedInCurrentThread");
        }
    });
    return {};
}

VoidResult InvokeBlockingFunctionFromAnotherThread(StringResultFuncView func, StringCallbackView callback)
{
    std::cout << "invokeBlockingFunctionFromAnotherThread called" << std::endl;
    std::thread thread([func = StringResultFunc(func), callback = StringCallback(callback)]() {
        std::cout << "invokeBlockingFunctionFromAnotherThread thread started" << std::endl;
        auto res = func();
        if (!res.has_value()) {
            std::cout << "invokeBlockingFunctionFromAnotherThread error: " << res.error().message() << std::endl;
            callback("<ERROR>");
        } else {
            std::cout << "invokeBlockingFunctionFromAnotherThread value: " << *res << std::endl;
            callback(*res + "-ProcessedInAnotherThread");
        }
    });
    thread.detach();
    return {};
}

VoidResult InvokeNonBlockingFunctionFromAnotherThread(StringFutureFuncView func, StringCallbackView callback)
{
    std::cout << "invokeNonBlockingFunctionFromAnotherThread called" << std::endl;
    std::thread thread([func = StringFutureFunc(func), callback = StringCallback(callback)]() {
        std::cout << "invokeNonBlockingFunctionFromAnotherThread thread started" << std::endl;
        auto fut = func();
        fut->on_complete([callback = StringCallback(callback)](StringResult res) {
            if (!res.has_value()) {
                std::cout << "invokeNonBlockingFunctionFromAnotherThread error: " << res.error().message() << std::endl;
                callback("<ERROR>");
            } else {
                std::cout << "invokeNonBlockingFunctionFromAnotherThread value: " << *res << std::endl;
                callback(*res + "-ProcessedInAnotherThread");
            }
        });
    });
    thread.detach();
    return {};
}

VoidResult InvokeBlockingMethodFromCurrentThread(weak::BlockingInvokable obj, StringCallbackView callback)
{
    std::cout << "invokeBlockingMethodFromCurrentThread called" << std::endl;
    auto res = obj->invoke();
    if (!res.has_value()) {
        std::cout << "invokeBlockingMethodFromCurrentThread error: " << res.error().message() << std::endl;
        callback("<ERROR>");
    } else {
        std::cout << "invokeBlockingMethodFromCurrentThread value: " << *res << std::endl;
        callback(*res + "-ProcessedMethodInCurrentThread");
    }
    return {};
}

VoidResult InvokeNonBlockingMethodFromCurrentThread(weak::NonBlockingInvokable obj, StringCallbackView callback)
{
    std::cout << "invokeNonBlockingMethodFromCurrentThread called" << std::endl;
    auto fut = obj->invoke();
    fut->on_complete([callback = StringCallback(callback)](StringResult res) {
        if (!res.has_value()) {
            std::cout << "invokeNonBlockingMethodFromCurrentThread error: " << res.error().message() << std::endl;
            callback("<ERROR>");
        } else {
            std::cout << "invokeNonBlockingMethodFromCurrentThread value: " << *res << std::endl;
            callback(*res + "-ProcessedMethodInCurrentThread");
        }
    });
    return {};
}

VoidResult InvokeBlockingMethodFromAnotherThread(weak::BlockingInvokable obj, StringCallbackView callback)
{
    std::cout << "invokeBlockingMethodFromAnotherThread called" << std::endl;
    std::thread thread([obj = BlockingInvokable(obj), callback = StringCallback(callback)]() {
        std::cout << "invokeBlockingMethodFromAnotherThread thread started" << std::endl;
        auto res = obj->invoke();
        if (!res.has_value()) {
            std::cout << "invokeBlockingMethodFromAnotherThread error: " << res.error().message() << std::endl;
            callback("<ERROR>");
        } else {
            std::cout << "invokeBlockingMethodFromAnotherThread value: " << *res << std::endl;
            callback(*res + "-ProcessedMethodInAnotherThread");
        }
    });
    thread.detach();
    return {};
}

VoidResult InvokeNonBlockingMethodFromAnotherThread(weak::NonBlockingInvokable obj, StringCallbackView callback)
{
    std::cout << "invokeNonBlockingMethodFromAnotherThread called" << std::endl;
    std::thread thread([obj = NonBlockingInvokable(obj), callback = StringCallback(callback)]() {
        std::cout << "invokeNonBlockingMethodFromAnotherThread thread started" << std::endl;
        auto fut = obj->invoke();
        fut->on_complete([callback = StringCallback(callback)](StringResult res) {
            if (!res.has_value()) {
                std::cout << "invokeNonBlockingMethodFromAnotherThread error: " << res.error().message() << std::endl;
                callback("<ERROR>");
            } else {
                std::cout << "invokeNonBlockingMethodFromAnotherThread value: " << *res << std::endl;
                callback(*res + "-ProcessedMethodInAnotherThread");
            }
        });
    });
    thread.detach();
    return {};
}

}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_InvokeBlockingFunctionFromCurrentThread(InvokeBlockingFunctionFromCurrentThread);
TH_EXPORT_CPP_API_InvokeNonBlockingFunctionFromCurrentThread(InvokeNonBlockingFunctionFromCurrentThread);
TH_EXPORT_CPP_API_InvokeBlockingFunctionFromAnotherThread(InvokeBlockingFunctionFromAnotherThread);
TH_EXPORT_CPP_API_InvokeNonBlockingFunctionFromAnotherThread(InvokeNonBlockingFunctionFromAnotherThread);
TH_EXPORT_CPP_API_InvokeBlockingMethodFromCurrentThread(InvokeBlockingMethodFromCurrentThread);
TH_EXPORT_CPP_API_InvokeNonBlockingMethodFromCurrentThread(InvokeNonBlockingMethodFromCurrentThread);
TH_EXPORT_CPP_API_InvokeBlockingMethodFromAnotherThread(InvokeBlockingMethodFromAnotherThread);
TH_EXPORT_CPP_API_InvokeNonBlockingMethodFromAnotherThread(InvokeNonBlockingMethodFromAnotherThread);
// NOLINTEND
