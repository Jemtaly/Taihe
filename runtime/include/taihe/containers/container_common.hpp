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

#ifndef TAIHE_CONTAINERS_CONTAINER_COMMON_HPP
#define TAIHE_CONTAINERS_CONTAINER_COMMON_HPP

#include <mutex>
#include <shared_mutex>
#include <type_traits>
#include <utility>

#include <taihe/common.hpp>

namespace taihe {
// Sentinel type used to mark the end of an iterable range.
struct end_sentinel_t {};

constexpr inline end_sentinel_t end_sentinel {};

// Dummy guard type for single-threaded containers (no-op lock).
struct null_lock_guard_t {};

constexpr inline null_lock_guard_t null_lock_guard {};

// Threading policy: no locking, caller guarantees no concurrent access.
struct single_thread {
    [[nodiscard]] null_lock_guard_t lock_unique() const
    {
        return null_lock_guard;
    }

    [[nodiscard]] null_lock_guard_t lock_shared() const
    {
        return null_lock_guard;
    }
};

// Threading policy: internal locking using Mutex.
struct multi_thread {
    using mutex_type = std::shared_mutex;
    using unique_guard = std::unique_lock<mutex_type>;
    using shared_guard = std::shared_lock<mutex_type>;

    [[nodiscard]] unique_guard lock_unique() const
    {
        return unique_guard(mtx);
    }

    [[nodiscard]] shared_guard lock_shared() const
    {
        return shared_guard(mtx);
    }

private:
    mutable mutex_type mtx;
};

// Helper trait: make a type assignable by stripping const/reference.
template<typename T>
struct as_assignable {
    using type = std::remove_cv_t<std::remove_reference_t<T>>;
};

// Specialization for std::pair: strip const from both key/value.
template<typename K, typename V>
struct as_assignable<std::pair<K, V>> {
    using type = std::pair<std::remove_const_t<K>, std::remove_const_t<V>>;
};

template<typename T>
struct as_assignable<T const> : as_assignable<T> {};

template<typename T>
struct as_assignable<T &> : as_assignable<T> {};

template<typename T>
struct as_assignable<T &&> : as_assignable<T> {};

template<typename T>
using as_assignable_t = typename as_assignable<T>::type;
}  // namespace taihe
#endif  // TAIHE_CONTAINERS_CONTAINER_COMMON_HPP