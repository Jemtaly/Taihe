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

#ifndef TAIHE_ITERATOR_HPP
#define TAIHE_ITERATOR_HPP

#include <cstddef>
#include <iterator>

#include <taihe/containers/container_common.hpp>
#include <taihe/iterator.proj.hpp>

namespace taihe {
template<typename T>
struct iterator {
    using mutex_type = std::shared_mutex;
    using value_type = T;
    using iterator_category = std::input_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using reference = value_type;
    using pointer = void;

    explicit iterator(::taihe::IIterator<value_type> &&iterator_object) : iiterator_object(std::move(iterator_object))
    {
    }

    value_type get() const
    {
        return *static_cast<value_type const *>(iiterator_object.m_handle.vtbl_ptr->get(iiterator_object.m_handle));
    }

    void move_next()
    {
        iiterator_object.m_handle.vtbl_ptr->move_next(iiterator_object.m_handle);
    }

    bool is_end() const
    {
        return iiterator_object.m_handle.vtbl_ptr->is_end(iiterator_object.m_handle);
    }

    reference operator*() const
    {
        return get();
    }

    iterator &operator++()
    {
        move_next();
        return *this;
    }

private:
    ::taihe::IIterator<value_type> iiterator_object;

    friend bool operator==(iterator const &lhs, iterator const &rhs)
    {
        return lhs.iiterator_object == rhs.iiterator_object;
    }

    friend bool operator!=(iterator const &lhs, iterator const &rhs)
    {
        return !(lhs == rhs);
    }

    friend bool operator!=(iterator const &lhs, ::taihe::end_sentinel_t const &)
    {
        return !lhs.is_end();
    }

    friend bool operator==(iterator const &lhs, ::taihe::end_sentinel_t const &)
    {
        return lhs.is_end();
    }
};

template<typename T>
struct as_abi<iterator<T>> {
    using type = TIterator;
};
}  // namespace taihe
#endif  // TAIHE_ITERATOR_HPP