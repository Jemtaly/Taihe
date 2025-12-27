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

#ifndef TAIHE_ITERATOR_IMPL_HPP
#define TAIHE_ITERATOR_IMPL_HPP

#include <optional>
#include <stdexcept>

#include <taihe/object.hpp>

namespace taihe {
template<typename T, typename ContainerImpl, typename Container>
struct IteratorImpl {
    using const_iterator = typename Container::const_iterator;

    explicit IteratorImpl(const_iterator iter, ContainerImpl const *container_impl)
        : inner_iter(iter), container(container_impl), expected_epoch(container_impl->current_epoch())
    {
        if (inner_iter != container->end()) {
            value_slot.emplace(*inner_iter);
        }
    }

    void const *get() const
    {
        [[maybe_unused]] auto guard = container->lock_shared();
        this->check_epoch();
        if (TH_UNLIKELY(inner_iter == container->end())) {
            TH_THROW(std::out_of_range, "Iterator at end");
        }
        return static_cast<void const *>(std::addressof(value_slot.value()));
    }

    void move_next()
    {
        [[maybe_unused]] auto guard = container->lock_unique();
        this->check_epoch();
        if (TH_UNLIKELY(inner_iter == container->end())) {
            TH_THROW(std::out_of_range, "Iterator at end");
        }
        ++inner_iter;
        if (TH_LIKELY(inner_iter != container->end())) {
            value_slot.emplace(*inner_iter);
        }
    }

    [[nodiscard]] bool is_end() const
    {
        [[maybe_unused]] auto guard = container->lock_shared();
        this->check_epoch();
        return inner_iter == container->end();
    }

    void check_epoch() const
    {
        if (TH_UNLIKELY(expected_epoch != container->current_epoch())) {
            TH_THROW(std::logic_error, "Structural modification");
        }
    }

private:
    const_iterator inner_iter;
    ContainerImpl const *container;
    std::uint64_t const expected_epoch;
    std::optional<T> value_slot;
};
}  // namespace taihe
#endif  // TAIHE_ITERATOR_IMPL_HPP