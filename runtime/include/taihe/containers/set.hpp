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

#ifndef TAIHE_CONTAINERS_SET_HPP
#define TAIHE_CONTAINERS_SET_HPP

#include <set>
#include <unordered_set>

#include <taihe/containers/container_base.hpp>
#include <taihe/containers/set.impl.hpp>
#include <taihe/containers/set.inner.hpp>
#include <taihe/containers/set.proj.hpp>
#include <taihe/iterator.hpp>
#include <taihe/object.hpp>

#define SET_DEFAULT_CAPACITY 16

namespace taihe {

template<typename K, typename Threading = ::taihe::single_thread>
struct set_view;

template<typename K, typename Threading = ::taihe::single_thread>
struct set;

template<class K>
using thread_safe_set_view = set_view<K, ::taihe::multi_thread>;

template<class K>
using thread_safe_set = set<K, ::taihe::multi_thread>;

template<typename K, typename Threading>
struct set_view : set_base<K, ::taihe::ISetView<K>> {
    using handle_type = ::taihe::ISetView<K>;

    explicit set_view(struct TSet m_handle) : set_base(handle_type(m_handle))
    {
    }

    explicit set_view(handle_type &&handle) : set_base(std::move(handle))
    {
    }

    set_view(set<K, Threading> const &holder) : set_base(handle_type(holder.i_container.m_handle))
    {
    }

private:
    using set_base = set_base<K, handle_type>;

    friend struct set<K, Threading>;
    friend struct std::hash<set<K, Threading>>;
};

template<typename K, typename Threading>
struct set : set_base<K, ::taihe::ISet<K>> {
    using handle_type = ::taihe::ISet<K>;

    explicit set(struct TSet m_handle) : set_base(handle_type(m_handle))
    {
    }

    explicit set(handle_type &&handle) : set_base(std::move(handle))
    {
    }

    explicit set(std::size_t cap = SET_DEFAULT_CAPACITY)
        : set_base(::taihe::make_holder<::taihe::SetImpl<K, ::taihe::set_inner<K>, Threading>, ::taihe::ISet<K>>(
              ::taihe::set_inner<K>(cap)))
    {
    }

    template<typename Hash, typename KeyEqual, typename Allocator>
    explicit set(std::unordered_set<K, Hash, KeyEqual, Allocator> &&container)
        : set_base(
              ::taihe::make_holder<::taihe::SetImpl<K, std::unordered_set<K, Hash, KeyEqual, Allocator>, Threading>,
                                   ::taihe::ISet<K>>(std::move(container)))
    {
    }

    template<typename Compare, typename Allocator>
    explicit set(std::set<K, Compare, Allocator> &&container)
        : set_base(
              ::taihe::make_holder<::taihe::SetImpl<K, std::set<K, Compare, Allocator>, Threading>, ::taihe::ISet<K>>(
                  std::move(container)))
    {
    }

    set(set_view<K, Threading> const &view) : set_base(handle_type(view.i_container))
    {
    }

private:
    using set_base = set_base<K, handle_type>;

    friend struct set_view<K, Threading>;
};

template<typename K>
struct as_abi<set<K>> {
    using type = TSet;
};

template<typename K>
struct as_abi<set_view<K>> {
    using type = TSet;
};

template<typename K>
struct as_abi<thread_safe_set<K>> {
    using type = TSet;
};

template<typename K>
struct as_abi<thread_safe_set_view<K>> {
    using type = TSet;
};

template<typename K>
struct as_param<set<K>> {
    using type = set_view<K>;
};

template<typename K>
struct as_param<thread_safe_set<K>> {
    using type = thread_safe_set_view<K>;
};

}  // namespace taihe

template<typename K>
struct std::hash<::taihe::set<K>> {
    std::size_t operator()(taihe::set_view<K> val) const noexcept
    {
        return ::std::hash<::taihe::data_holder>()(val.i_container);
    }
};

template<typename K>
struct std::hash<::taihe::thread_safe_set<K>> {
    std::size_t operator()(taihe::thread_safe_set_view<K> val) const noexcept
    {
        return ::std::hash<::taihe::data_holder>()(val.i_container);
    }
};

#undef SET_DEFAULT_CAPACITY

#endif  // TAIHE_CONTAINERS_SET_HPP
