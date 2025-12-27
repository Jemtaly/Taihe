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

#ifndef TAIHE_CONTAINERS_MAP_IMPL_HPP
#define TAIHE_CONTAINERS_MAP_IMPL_HPP

#include <type_traits>

#include <taihe/common.hpp>
#include <taihe/containers/container_common.hpp>
#include <taihe/iterator.impl.hpp>
#include <taihe/iterator.proj.hpp>
#include <taihe/object.hpp>

namespace taihe {

template<typename K, typename V, typename Container, typename Threading>
struct MapImpl : Threading {
    using Threading::lock_shared;
    using Threading::lock_unique;
    using value_type = std::pair<K const, V>;
    using size_type = std::size_t;
    using IteratorImpl = IteratorImpl<value_type, MapImpl<K, V, Container, Threading>, Container>;

    explicit MapImpl(Container &&container) noexcept(std::is_nothrow_move_constructible_v<Container>)
        : inner_map(std::forward<Container>(container)), mod_epoch(0)
    {
    }

    void *get(void const *key) const
    {
        [[maybe_unused]] auto guard = lock_shared();
        auto it = inner_map.find(*static_cast<as_assignable_t<as_param_t<K>> const *>(key));
        if (TH_UNLIKELY(it == inner_map.end())) {
            TH_THROW(std::out_of_range, "Key not found in map");
        }
        return static_cast<void *>(&it->second);
    }

    void set(void const *key, void const *val)
    {
        [[maybe_unused]] auto guard = lock_unique();
        auto [it, inserted] = inner_map.insert_or_assign(*static_cast<as_assignable_t<as_param_t<K>> const *>(key),
                                                         *static_cast<as_assignable_t<as_param_t<V>> const *>(val));
        if (inserted) {
            this->bump_epoch();
        }
    }

    bool insert(void const *key, void const *val)
    {
        [[maybe_unused]] auto guard = lock_unique();
        auto [it, inserted] = inner_map.emplace(*static_cast<as_assignable_t<as_param_t<K>> const *>(key),
                                                *static_cast<as_assignable_t<as_param_t<V>> const *>(val));
        if (inserted) {
            this->bump_epoch();
        }
        return inserted;
    }

    bool remove(void const *key)
    {
        [[maybe_unused]] auto guard = lock_unique();
        bool removed = inner_map.erase(*static_cast<as_assignable_t<as_param_t<K>> const *>(key)) > 0;
        if (TH_LIKELY(removed)) {
            this->bump_epoch();
        }
        return removed;
    }

    void clear()
    {
        [[maybe_unused]] auto guard = lock_unique();
        this->bump_epoch();
        inner_map.clear();
    }

    bool contains(void const *key) const
    {
        [[maybe_unused]] auto guard = lock_shared();
        return inner_map.find(*static_cast<as_assignable_t<as_param_t<K>> const *>(key)) != inner_map.end();
    }

    size_type size() const noexcept
    {
        [[maybe_unused]] auto guard = lock_shared();
        return inner_map.size();
    }

    void bump_epoch() noexcept
    {
        ++mod_epoch;
    }

    std::uint64_t current_epoch() const noexcept
    {
        return mod_epoch;
    }

    ::taihe::IIterator<value_type> iter() const
    {
        [[maybe_unused]] auto guard = lock_shared();
        return ::taihe::make_holder<IteratorImpl, ::taihe::IIterator<value_type>>(inner_map.cbegin(), this);
    }

    typename Container::const_iterator end() const noexcept
    {
        return inner_map.cend();
    }

private:
    Container inner_map;
    std::uint64_t mod_epoch;
};

}  // namespace taihe
#endif  // TAIHE_CONTAINERS_MAP_IMPL_HPP