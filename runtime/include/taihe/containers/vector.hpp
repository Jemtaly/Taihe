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

#ifndef TAIHE_CONTAINERS_VECTOR_HPP
#define TAIHE_CONTAINERS_VECTOR_HPP

#include <vector>

#include <taihe/containers/container_base.hpp>
#include <taihe/containers/vector.impl.hpp>
#include <taihe/containers/vector.inner.hpp>
#include <taihe/containers/vector.proj.hpp>
#include <taihe/iterator.hpp>
#include <taihe/object.hpp>

#define VECTOR_DEFAULT_CAPACITY 16

namespace taihe {

template<typename V, typename Threading = ::taihe::single_thread>
struct vector_view;

template<typename V, typename Threading = ::taihe::single_thread>
struct vector;

template<class V>
using thread_safe_vector_view = vector_view<V, ::taihe::multi_thread>;

template<class V>
using thread_safe_vector = vector<V, ::taihe::multi_thread>;

template<typename V, typename Threading>
struct vector_view : vector_base<V, ::taihe::IVectorView<V>> {
    using handle_type = ::taihe::IVectorView<V>;
    using vector_base<V, handle_type>::npos;

    explicit vector_view(struct TVector m_handle) : vector_base(handle_type(m_handle))
    {
    }

    explicit vector_view(handle_type &&handle) : vector_base(std::move(handle))
    {
    }

    vector_view(vector<V, Threading> const &holder) : vector_base(handle_type(holder.i_container.m_handle))
    {
    }

private:
    using vector_base = vector_base<V, handle_type>;

    friend struct vector<V, Threading>;
    friend struct std::hash<vector<V, Threading>>;
};

template<typename V, typename Threading>
struct vector : vector_base<V, ::taihe::IVector<V>> {
    using handle_type = ::taihe::IVector<V>;
    using vector_base<V, handle_type>::npos;

    explicit vector(struct TVector m_handle) : vector_base(handle_type(m_handle))
    {
    }

    explicit vector(handle_type &&handle) : vector_base(std::move(handle))
    {
    }

    explicit vector(std::size_t cap = VECTOR_DEFAULT_CAPACITY)
        : vector_base(
              ::taihe::make_holder<::taihe::VectorImpl<V, ::taihe::vector_inner<V>, Threading>, ::taihe::IVector<V>>(
                  ::taihe::vector_inner<V>(cap)))
    {
    }

    template<typename Allocator>
    explicit vector(::std::vector<V, Allocator> &&container)
        : vector_base(
              ::taihe::make_holder<::taihe::VectorImpl<V, ::std::vector<V, Allocator>, Threading>, ::taihe::IVector<V>>(
                  std::move(container)))
    {
    }

    vector(vector_view<V, Threading> const &view) : vector_base(handle_type(view.i_container))
    {
    }

private:
    using vector_base = vector_base<V, handle_type>;

    friend struct vector_view<V, Threading>;
};

template<typename V>
struct as_abi<vector<V>> {
    using type = TVector;
};

template<typename V>
struct as_abi<vector_view<V>> {
    using type = TVector;
};

template<typename V>
struct as_abi<thread_safe_vector<V>> {
    using type = TVector;
};

template<typename V>
struct as_abi<thread_safe_vector_view<V>> {
    using type = TVector;
};

template<typename V>
struct as_param<vector<V>> {
    using type = vector_view<V>;
};

template<typename V>
struct as_param<thread_safe_vector<V>> {
    using type = thread_safe_vector_view<V>;
};

}  // namespace taihe

template<typename V>
struct std::hash<::taihe::vector<V>> {
    std::size_t operator()(taihe::vector_view<V> val) const noexcept
    {
        return ::std::hash<::taihe::data_holder>()(val.i_container);
    }
};

template<typename V>
struct std::hash<::taihe::thread_safe_vector<V>> {
    std::size_t operator()(taihe::thread_safe_vector_view<V> val) const noexcept
    {
        return ::std::hash<::taihe::data_holder>()(val.i_container);
    }
};

#undef VECTOR_DEFAULT_CAPACITY

#endif  // TAIHE_CONTAINERS_VECTOR_HPP
