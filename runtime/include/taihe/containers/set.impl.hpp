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

#ifndef TAIHE_CONTAINERS_SET_IMPL_HPP
#define TAIHE_CONTAINERS_SET_IMPL_HPP

#include <type_traits>

#include <taihe/common.hpp>
#include <taihe/containers/container_base.hpp>
#include <taihe/iterator.impl.hpp>
#include <taihe/iterator.proj.hpp>
#include <taihe/object.hpp>

namespace taihe {

template<typename K, typename Container, typename Threading>
struct SetImpl : Threading {
    using Threading::lock_shared;
    using Threading::lock_unique;
    using value_type = K;
    using size_type = std::size_t;
    using IteratorImpl = IteratorImpl<value_type, SetImpl<K, Container, Threading>, Container>;

    explicit SetImpl(Container &&container) noexcept(std::is_nothrow_move_constructible_v<Container>)
        : inner_set(std::forward<Container>(container)), mod_epoch(0)
    {
    }

    bool insert(void const *key)
    {
        [[maybe_unused]] auto guard = lock_unique();
        auto [it, inserted] = inner_set.emplace(*static_cast<as_assignable_t<as_param_t<K>> const *>(key));
        if (inserted) {
            this->bump_epoch();
        }
        return inserted;
    }

    bool remove(void const *key)
    {
        [[maybe_unused]] auto guard = lock_unique();
        bool removed = inner_set.erase(*static_cast<as_assignable_t<as_param_t<K>> const *>(key)) > 0;
        if (TH_LIKELY(removed)) {
            this->bump_epoch();
        }
        return removed;
    }

    void clear()
    {
        [[maybe_unused]] auto guard = lock_unique();
        this->bump_epoch();
        inner_set.clear();
    }

    bool contains(void const *key) const
    {
        [[maybe_unused]] auto guard = lock_shared();
        return inner_set.find(*static_cast<as_assignable_t<as_param_t<K>> const *>(key)) != inner_set.end();
    }

    size_type size() const noexcept
    {
        [[maybe_unused]] auto guard = lock_shared();
        return inner_set.size();
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
        return ::taihe::make_holder<IteratorImpl, ::taihe::IIterator<value_type>>(inner_set.cbegin(), this);
    }

    typename Container::const_iterator end() const noexcept
    {
        return inner_set.cend();
    }

private:
    Container inner_set;
    std::uint64_t mod_epoch;
};

}  // namespace taihe

#endif  // TAIHE_CONTAINERS_SET_IMPL_HPP
