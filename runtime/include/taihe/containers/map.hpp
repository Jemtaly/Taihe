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

#ifndef TAIHE_CONTAINERS_MAP_HPP
#define TAIHE_CONTAINERS_MAP_HPP

#include <map>
#include <unordered_map>

#include <taihe/containers/container_base.hpp>
#include <taihe/containers/map.impl.hpp>
#include <taihe/containers/map.inner.hpp>
#include <taihe/containers/map.proj.hpp>
#include <taihe/iterator.hpp>
#include <taihe/object.hpp>

#define MAP_DEFAULT_CAPACITY 16

namespace taihe {

template<typename K, typename V, typename Threading = ::taihe::single_thread>
struct map_view;

template<typename K, typename V, typename Threading = ::taihe::single_thread>
struct map;

template<class K, class V>
using thread_safe_map_view = map_view<K, V, ::taihe::multi_thread>;

template<class K, class V>
using thread_safe_map = map<K, V, ::taihe::multi_thread>;

template<typename K, typename V, typename Threading>
struct map_view : map_base<K, V, ::taihe::IMapView<K, V>> {
    using handle_type = ::taihe::IMapView<K, V>;

    explicit map_view(struct TMap m_handle) : map_base(handle_type(m_handle))
    {
    }

    explicit map_view(handle_type &&handle) : map_base(std::move(handle))
    {
    }

    map_view(map<K, V, Threading> const &holder) : map_base(handle_type(holder.i_container.m_handle))
    {
    }

private:
    using map_base = map_base<K, V, handle_type>;

    friend struct map<K, V, Threading>;
    friend struct std::hash<map<K, V, Threading>>;
};

template<typename K, typename V, typename Threading>
struct map : map_base<K, V, ::taihe::IMap<K, V>> {
    using handle_type = ::taihe::IMap<K, V>;

    explicit map(struct TMap m_handle) : map_base(handle_type(m_handle))
    {
    }

    explicit map(handle_type &&handle) : map_base(std::move(handle))
    {
    }

    explicit map(std::size_t cap = MAP_DEFAULT_CAPACITY)
        : map_base(
              ::taihe::make_holder<::taihe::MapImpl<K, V, ::taihe::map_inner<K, V>, Threading>, ::taihe::IMap<K, V>>(
                  ::taihe::map_inner<K, V>(cap)))
    {
    }

    template<typename Hash, typename KeyEqual, typename Allocator>
    explicit map(std::unordered_map<K, V, Hash, KeyEqual, Allocator> &&container)
        : map_base(::taihe::make_holder<
                   ::taihe::MapImpl<K, V, std::unordered_map<K, V, Hash, KeyEqual, Allocator>, Threading>,
                   ::taihe::IMap<K, V>>(std::move(container)))
    {
    }

    template<typename Compare, typename Allocator>
    explicit map(std::map<K, V, Compare, Allocator> &&container)
        : map_base(::taihe::make_holder<::taihe::MapImpl<K, V, std::map<K, V, Compare, Allocator>, Threading>,
                                        ::taihe::IMap<K, V>>(std::move(container)))
    {
    }

    map(map_view<K, V, Threading> const &view) : map_base(handle_type(view.i_container))
    {
    }

private:
    using map_base = map_base<K, V, handle_type>;

    friend struct map_view<K, V, Threading>;
};

template<typename K, typename V>
struct as_abi<map<K, V>> {
    using type = TMap;
};

template<typename K, typename V>
struct as_abi<map_view<K, V>> {
    using type = TMap;
};

template<typename K, typename V>
struct as_abi<thread_safe_map<K, V>> {
    using type = TMap;
};

template<typename K, typename V>
struct as_abi<thread_safe_map_view<K, V>> {
    using type = TMap;
};

template<typename K, typename V>
struct as_param<map<K, V>> {
    using type = map_view<K, V>;
};

template<typename K, typename V>
struct as_param<thread_safe_map<K, V>> {
    using type = thread_safe_map_view<K, V>;
};
}  // namespace taihe

template<typename K, typename V>
struct std::hash<::taihe::map<K, V>> {
    std::size_t operator()(taihe::map_view<K, V> val) const noexcept
    {
        return ::std::hash<::taihe::data_holder>()(val.i_container);
    }
};

template<typename K, typename V>
struct std::hash<::taihe::thread_safe_map<K, V>> {
    std::size_t operator()(taihe::thread_safe_map_view<K, V> val) const noexcept
    {
        return ::std::hash<::taihe::data_holder>()(val.i_container);
    }
};

#undef MAP_DEFAULT_CAPACITY

#endif  // TAIHE_CONTAINERS_MAP_HPP
