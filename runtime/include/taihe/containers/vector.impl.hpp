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

#ifndef TAIHE_CONTAINERS_VECTOR_IMPL_HPP
#define TAIHE_CONTAINERS_VECTOR_IMPL_HPP

#include <algorithm>
#include <cstdint>
#include <type_traits>

#include <taihe/common.hpp>
#include <taihe/containers/container_common.hpp>
#include <taihe/iterator.impl.hpp>
#include <taihe/iterator.proj.hpp>
#include <taihe/object.hpp>

namespace taihe {

template<typename V, typename Container, typename Threading>
struct VectorImpl : Threading {
    using Threading::lock_shared;
    using Threading::lock_unique;

    using value_type = V;
    using size_type = std::size_t;

    static constexpr size_type npos = static_cast<size_type>(-1);

    using IteratorImpl = IteratorImpl<value_type, VectorImpl<V, Container, Threading>, Container>;

    explicit VectorImpl(Container &&container) noexcept(std::is_nothrow_move_constructible_v<Container>)
        : inner_vec(std::forward<Container>(container)), mod_epoch(0)
    {
    }

    void *at(std::uint64_t idx) const
    {
        [[maybe_unused]] auto guard = lock_shared();
        if (TH_UNLIKELY(idx >= inner_vec.size())) {
            TH_THROW(std::out_of_range, "Index out of range");
        }
        return static_cast<void *>(&inner_vec[static_cast<size_type>(idx)]);
    }

    void set(std::uint64_t idx, void const *val)
    {
        [[maybe_unused]] auto guard = lock_unique();
        if (TH_UNLIKELY(idx >= inner_vec.size())) {
            TH_THROW(std::out_of_range, "Index out of range");
        }
        inner_vec[static_cast<size_type>(idx)] = *static_cast<as_assignable_t<as_param_t<V>> const *>(val);
    }

    void insert(std::uint64_t idx, void const *val)
    {
        [[maybe_unused]] auto guard = lock_unique();
        if (TH_UNLIKELY(idx > inner_vec.size())) {
            TH_THROW(std::out_of_range, "Index out of range");
        }
        inner_vec.insert(inner_vec.begin() + static_cast<size_type>(idx),
                         *static_cast<as_assignable_t<as_param_t<V>> const *>(val));
        this->bump_epoch();
    }

    void push_back(void const *val)
    {
        [[maybe_unused]] auto guard = lock_unique();
        inner_vec.push_back(*static_cast<as_assignable_t<as_param_t<V>> const *>(val));
        this->bump_epoch();
    }

    void pop_back()
    {
        [[maybe_unused]] auto guard = lock_unique();
        if (TH_UNLIKELY(inner_vec.empty())) {
            TH_THROW(std::out_of_range, "Vector is empty");
        }
        inner_vec.pop_back();
        this->bump_epoch();
    }

    void remove(std::uint64_t idx)
    {
        [[maybe_unused]] auto guard = lock_unique();
        if (TH_UNLIKELY(idx >= inner_vec.size())) {
            TH_THROW(std::out_of_range, "Index out of range");
        }
        inner_vec.erase(inner_vec.begin() + static_cast<size_type>(idx));
        this->bump_epoch();
    }

    void clear()
    {
        [[maybe_unused]] auto guard = lock_unique();
        this->bump_epoch();
        inner_vec.clear();
    }

    size_type size() const noexcept
    {
        [[maybe_unused]] auto guard = lock_shared();
        return inner_vec.size();
    }

    void fill(void const *val)
    {
        [[maybe_unused]] auto guard = lock_unique();
        auto const &v = *static_cast<as_assignable_t<as_param_t<V>> const *>(val);
        std::fill(inner_vec.begin(), inner_vec.end(), v);
    }

    std::uint64_t find(void const *val, std::uint64_t idx) const
    {
        [[maybe_unused]] auto guard = lock_shared();
        if (TH_UNLIKELY(idx >= inner_vec.size())) {
            return static_cast<std::uint64_t>(npos);
        }
        auto const &target = *static_cast<as_assignable_t<as_param_t<V>> const *>(val);

        auto first = inner_vec.cbegin() + static_cast<size_type>(idx);
        auto it = std::find(first, inner_vec.cend(), target);
        return it == inner_vec.cend() ? static_cast<std::uint64_t>(npos)
                                      : static_cast<std::uint64_t>(it - inner_vec.cbegin());
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
        return ::taihe::make_holder<IteratorImpl, ::taihe::IIterator<value_type>>(inner_vec.cbegin(), this);
    }

    typename Container::const_iterator end() const noexcept
    {
        return inner_vec.cend();
    }

private:
    Container inner_vec;
    std::uint64_t mod_epoch;
};
}  // namespace taihe

#endif  // TAIHE_CONTAINERS_VECTOR_IMPL_HPP
