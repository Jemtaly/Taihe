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

#ifndef TAIHE_CONTAINERS_CONTAINER_BASE_HPP
#define TAIHE_CONTAINERS_CONTAINER_BASE_HPP

#include <taihe/common.hpp>
#include <taihe/containers/container_common.hpp>
#include <taihe/iterator.proj.hpp>

namespace taihe {
// Decl of iterators.
template<typename T>
struct iterator;

// Base wrapper for iterable containers (map, set, vector, ...).
// - T:      projected element type
// - Handle: ABI handle type (e.g. IMap<K, V>, IMapView<K, V>, ISet<K>, ...)
template<typename T, typename Handle>
struct iterable {
    iterable(iterable const &) = default;
    iterable(iterable &&) = default;
    iterable &operator=(iterable const &) = default;
    iterable &operator=(iterable &&) = default;
    ~iterable() = default;

    using iterator = ::taihe::iterator<T>;
    using const_iterator = iterator;

    // Acquire a shared lock and create a projected iterator.
    iterator iter() const
    {
        return iterator(
            ::taihe::from_abi<::taihe::IIterator<T>>(i_container.m_handle.vtbl_ptr->iter(i_container.m_handle)));
    }

    iterator begin() const
    {
        return this->iter();
    }

    ::taihe::end_sentinel_t end() const
    {
        return end_sentinel;
    }

    const_iterator cbegin() const
    {
        return this->iter();
    }

    ::taihe::end_sentinel_t cend() const
    {
        return end_sentinel;
    }

protected:
    // Underlying ABI handle, provided by the FFI layer.
    Handle i_container;

    explicit iterable(Handle &&handle) : i_container(std::move(handle))
    {
    }

    friend bool operator==(iterable const &lhs, iterable const &rhs)
    {
        return lhs.i_container == rhs.i_container;
    }

    friend bool operator!=(iterable const &lhs, iterable const &rhs)
    {
        return !(lhs == rhs);
    }
};

// Base wrapper for map-like containers.
// - K:        key type (logical type seen by C++)
// - V:        mapped type
// - Handle:   ABI handle type (IMap<K, V> or IMapView<K, V>)
template<typename K, typename V, typename Handle>
struct map_base : iterable<std::pair<K const, V>, Handle> {
    using key_type = K;
    using mapped_type = V;
    using value_type = std::pair<K const, V>;

    V get(as_param_t<K> key) const
    {
        return *static_cast<V *>(
            this->i_container.m_handle.vtbl_ptr->get(this->i_container.m_handle, static_cast<void const *>(&key)));
    }

    V operator[](as_param_t<K> key) const
    {
        return get(key);
    }

    void set(as_param_t<K> key, as_param_t<V> val)
    {
        this->i_container.m_handle.vtbl_ptr->set(this->i_container.m_handle, static_cast<void const *>(&key),
                                                 static_cast<void const *>(&val));
    }

    bool insert(as_param_t<K> key, as_param_t<V> val)
    {
        return this->i_container.m_handle.vtbl_ptr->insert(this->i_container.m_handle, static_cast<void const *>(&key),
                                                           static_cast<void const *>(&val));
    }

    bool remove(as_param_t<K> key)
    {
        return this->i_container.m_handle.vtbl_ptr->remove(this->i_container.m_handle, static_cast<void const *>(&key));
        ;
    }

    bool empty() const
    {
        return size() == 0;
    }

    void clear()
    {
        this->i_container.m_handle.vtbl_ptr->clear(this->i_container.m_handle);
    }

    bool contains(as_param_t<K> key) const
    {
        return this->i_container.m_handle.vtbl_ptr->contains(this->i_container.m_handle,
                                                             static_cast<void const *>(&key));
    }

    std::size_t size() const
    {
        return this->i_container.m_handle.vtbl_ptr->size(this->i_container.m_handle);
    }

protected:
    using iterable = iterable<value_type, Handle>;

    explicit map_base(Handle &&handle) : iterable(std::move(handle))
    {
    }
};

// Base wrapper for set-like containers.
// - K:      element type (logical type seen by C++)
// - Handle: ABI handle type (ISet<K> or ISetView<K>)
template<typename K, typename Handle>
struct set_base : iterable<K, Handle> {
    using key_type = K;
    using value_type = K;

    bool insert(as_param_t<K> key)
    {
        return this->i_container.m_handle.vtbl_ptr->insert(this->i_container.m_handle, static_cast<void const *>(&key));
    }

    bool remove(as_param_t<K> key)
    {
        return this->i_container.m_handle.vtbl_ptr->remove(this->i_container.m_handle, static_cast<void const *>(&key));
    }

    bool contains(as_param_t<K> key) const
    {
        return this->i_container.m_handle.vtbl_ptr->contains(this->i_container.m_handle,
                                                             static_cast<void const *>(&key));
    }

    bool empty() const
    {
        return size() == 0;
    }

    void clear()
    {
        this->i_container.m_handle.vtbl_ptr->clear(this->i_container.m_handle);
    }

    std::size_t size() const
    {
        return this->i_container.m_handle.vtbl_ptr->size(this->i_container.m_handle);
    }

protected:
    using iterable = iterable<value_type, Handle>;

    explicit set_base(Handle &&handle) : iterable(std::move(handle))
    {
    }
};

// Base wrapper for vector-like containers.
// - V:        element type (logical type seen by C++)
// - Handle:   ABI handle type (IVector<V> or IVectorView<V>)
template<typename V, typename Handle>
struct vector_base : iterable<V, Handle> {
    using value_type = V;
    using size_type = std::size_t;

    static constexpr inline size_type npos = static_cast<size_type>(-1);

    V at(size_type idx) const
    {
        return *static_cast<V *>(this->i_container.m_handle.vtbl_ptr->at(this->i_container.m_handle, idx));
    }

    V operator[](size_type idx) const
    {
        return at(idx);
    }

    void set(size_type idx, as_param_t<V> val)
    {
        this->i_container.m_handle.vtbl_ptr->set(this->i_container.m_handle, idx, static_cast<void const *>(&val));
    }

    void insert(size_type idx, as_param_t<V> val)
    {
        this->i_container.m_handle.vtbl_ptr->insert(this->i_container.m_handle, idx, static_cast<void const *>(&val));
    }

    void push_back(as_param_t<V> val)
    {
        this->i_container.m_handle.vtbl_ptr->push_back(this->i_container.m_handle, static_cast<void const *>(&val));
    }

    void pop_back()
    {
        this->i_container.m_handle.vtbl_ptr->pop_back(this->i_container.m_handle);
    }

    void remove(size_type idx)
    {
        this->i_container.m_handle.vtbl_ptr->remove(this->i_container.m_handle, idx);
    }

    void clear()
    {
        this->i_container.m_handle.vtbl_ptr->clear(this->i_container.m_handle);
    }

    bool empty() const
    {
        return size() == 0;
    }

    size_type size() const
    {
        return this->i_container.m_handle.vtbl_ptr->size(this->i_container.m_handle);
    }

    void fill(as_param_t<V> val)
    {
        this->i_container.m_handle.vtbl_ptr->fill(this->i_container.m_handle, static_cast<void const *>(&val));
    }

    size_type find(as_param_t<V> val, size_type idx = 0) const
    {
        return this->i_container.m_handle.vtbl_ptr->find(this->i_container.m_handle, static_cast<void const *>(&val),
                                                         idx);
    }

protected:
    using iterable = iterable<value_type, Handle>;

    explicit vector_base(Handle &&handle) : iterable(std::move(handle))
    {
    }
};
}  // namespace taihe
#endif  // TAIHE_CONTAINERS_CONTAINER_BASE_HPP