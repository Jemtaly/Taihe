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

#ifndef TAIHE_CONTAINERS_VECTOR_INNER_HPP
#define TAIHE_CONTAINERS_VECTOR_INNER_HPP

#include <algorithm>
#include <utility>

#include <taihe/common.hpp>
#include <taihe/containers/vector.inner.abi.h>

#define VEC_GROWTH_FACTOR 2
#define VEC_DEFAULT_CAPACITY 0

namespace taihe {
template<typename T>
struct vector_inner_view;

template<typename T>
struct vector_inner;

template<typename T>
struct vector_inner_view {
public:
    using value_type = T;
    using size_type = std::size_t;
    using reference = T &;
    using pointer = T *;

    void reserve(std::size_t cap) const
    {
        if (cap < m_handle->len) {
            return;
        }
        m_handle->cap = cap;
        m_handle->buffer = reinterpret_cast<T *>(realloc(m_handle->buffer, sizeof(T) * cap));
    }

    std::size_t size() const noexcept
    {
        return m_handle->len;
    }

    bool empty() const noexcept
    {
        return m_handle->len == 0;
    }

    std::size_t capacity() const noexcept
    {
        return m_handle->cap;
    }

    template<typename... Args>
    T &emplace_back(Args &&...args) const
    {
        std::size_t required_cap = m_handle->len + 1;
        if (required_cap > m_handle->cap) {
            this->reserve(std::max(required_cap, m_handle->cap * VEC_GROWTH_FACTOR));
        }
        T &item = m_handle->buffer[m_handle->len];
        new (&item) T {std::forward<Args>(args)...};
        ++m_handle->len;
        return item;
    }

    T &push_back(T &&value) const
    {
        return emplace_back(std::move(value));
    }

    T &push_back(T const &value) const
    {
        return emplace_back(value);
    }

    T &operator[](std::size_t index) const
    {
        return m_handle->buffer[index];
    }

    void pop_back() const
    {
        if (m_handle->len == 0) {
            return;
        }
        std::destroy_at(&m_handle->buffer[m_handle->len - 1]);
        --m_handle->len;
    }

    void clear() const noexcept
    {
        for (std::size_t i = 0; i < m_handle->len; i++) {
            std::destroy_at(&m_handle->buffer[i]);
        }
        m_handle->len = 0;
    }

    using iterator = T *;
    using const_iterator = T const *;

    iterator insert(const_iterator pos, T const &value) const
    {
        std::size_t idx = static_cast<std::size_t>(pos - this->cbegin());

        std::size_t required_cap = m_handle->len + 1;
        if (required_cap > m_handle->cap) {
            reserve(std::max(required_cap, m_handle->cap * VEC_GROWTH_FACTOR));
        }

        T *buf = m_handle->buffer;
        if (idx == m_handle->len) {
            new (&buf[m_handle->len]) T(value);
            ++m_handle->len;
            return buf + idx;
        }

        // make one slot at end
        new (&buf[m_handle->len]) T(std::move(buf[m_handle->len - 1]));
        // shift right [idx, len-1) -> [idx+1, len)
        for (std::size_t i = m_handle->len - 1; i > idx; --i) {
            buf[i] = std::move(buf[i - 1]);
        }
        buf[idx] = value;
        ++m_handle->len;
        return buf + idx;
    }

    iterator insert(const_iterator pos, T &&value) const
    {
        std::size_t idx = static_cast<std::size_t>(pos - this->cbegin());

        std::size_t required_cap = m_handle->len + 1;
        if (required_cap > m_handle->cap) {
            reserve(std::max(required_cap, m_handle->cap * VEC_GROWTH_FACTOR));
        }

        T *buf = m_handle->buffer;
        if (idx == m_handle->len) {
            new (&buf[m_handle->len]) T(std::move(value));
            ++m_handle->len;
            return buf + idx;
        }

        new (&buf[m_handle->len]) T(std::move(buf[m_handle->len - 1]));
        for (std::size_t i = m_handle->len - 1; i > idx; --i) {
            buf[i] = std::move(buf[i - 1]);
        }
        buf[idx] = std::move(value);
        ++m_handle->len;
        return buf + idx;
    }

    iterator erase(const_iterator pos) const
    {
        std::size_t idx = static_cast<std::size_t>(pos - this->cbegin());

        T *buf = m_handle->buffer;
        std::destroy_at(&buf[idx]);
        for (std::size_t i = idx; i + 1 < m_handle->len; ++i) {
            buf[i] = std::move(buf[i + 1]);
        }
        std::destroy_at(&buf[m_handle->len - 1]);
        --m_handle->len;
        return buf + idx;
    }

    iterator erase(const_iterator first, const_iterator last) const
    {
        std::size_t f = static_cast<std::size_t>(first - this->cbegin());
        std::size_t l = static_cast<std::size_t>(last - this->cbegin());

        if (f == l) return m_handle->buffer + f;

        T *buf = m_handle->buffer;
        // destroy removed range
        for (std::size_t i = f; i < l; ++i) {
            std::destroy_at(&buf[i]);
        }
        // move tail left
        std::size_t cnt = l - f;
        for (std::size_t i = f; i + cnt < m_handle->len; ++i) {
            buf[i] = std::move(buf[i + cnt]);
        }
        // destroy trailing duplicates
        for (std::size_t i = m_handle->len - cnt; i < m_handle->len; ++i) {
            std::destroy_at(&buf[i]);
        }
        m_handle->len -= cnt;
        return buf + f;
    }

    iterator begin() const
    {
        return m_handle->buffer;
    }

    iterator end() const
    {
        return m_handle->buffer + m_handle->len;
    }

    const_iterator cbegin() const
    {
        return begin();
    }

    const_iterator cend() const
    {
        return end();
    }

protected:
    struct data_t {
        TRefCount count;
        std::size_t cap;
        T *buffer;
        std::size_t len;
    } *m_handle;

    explicit vector_inner_view(data_t *handle) : m_handle(handle)
    {
    }

    friend struct vector_inner<T>;

    friend struct std::hash<vector_inner<T>>;

    friend bool operator==(vector_inner_view lhs, vector_inner_view rhs)
    {
        return lhs.m_handle == rhs.m_handle;
    }
};

template<typename T>
struct vector_inner : vector_inner_view<T> {
    using typename vector_inner_view<T>::data_t;
    using vector_inner_view<T>::m_handle;

    explicit vector_inner(std::size_t cap = VEC_DEFAULT_CAPACITY) : vector_inner(new data_t)
    {
        tref_init(&m_handle->count, 1);
        m_handle->cap = cap;
        m_handle->buffer = reinterpret_cast<T *>(malloc(sizeof(T) * cap));
        m_handle->len = 0;
    }

    vector_inner(vector_inner<T> &&other) noexcept : vector_inner(other.m_handle)
    {
        other.m_handle = nullptr;
    }

    vector_inner(vector_inner<T> const &other) : vector_inner(other.m_handle)
    {
        if (m_handle) {
            tref_inc(&m_handle->count);
        }
    }

    vector_inner(vector_inner_view<T> const &other) : vector_inner(other.m_handle)
    {
        if (m_handle) {
            tref_inc(&m_handle->count);
        }
    }

    vector_inner &operator=(vector_inner other)
    {
        std::swap(this->m_handle, other.m_handle);
        return *this;
    }

    ~vector_inner()
    {
        if (m_handle && tref_dec(&m_handle->count)) {
            this->clear();
            free(m_handle->buffer);
            delete m_handle;
        }
    }

private:
    explicit vector_inner(data_t *handle) : vector_inner_view<T>(handle)
    {
    }
};

template<typename T>
struct as_abi<vector_inner<T>> {
    using type = TVectorInner;
};

template<typename T>
struct as_abi<vector_inner_view<T>> {
    using type = TVectorInner;
};

template<typename T>
struct as_param<vector_inner<T>> {
    using type = vector_inner_view<T>;
};
}  // namespace taihe

template<typename T>
struct std::hash<taihe::vector_inner<T>> {
    std::size_t operator()(taihe::vector_inner_view<T> val) const noexcept
    {
        return reinterpret_cast<std::size_t>(val.m_handle);
    }
};

#undef VEC_GROWTH_FACTOR
#undef VEC_DEFAULT_CAPACITY

#endif  // TAIHE_CONTAINERS_VECTOR_INNER_HPP