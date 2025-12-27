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

#ifndef TAIHE_CONTAINERS_VECTOR_PROJ_HPP
#define TAIHE_CONTAINERS_VECTOR_PROJ_HPP

#include <utility>

#include <taihe/containers/vector.abi.h>
#include <taihe/iterator.proj.hpp>

namespace taihe {

template<typename V>
struct IVector;

template<typename V>
struct IVectorView {
    using value_type = V;
    static constexpr bool is_holder = false;
    struct TVector m_handle;

    explicit IVectorView(struct TVector handle) : m_handle(handle)
    {
    }

    operator ::taihe::data_view() const &
    {
        return ::taihe::data_view(this->m_handle.data_ptr);
    }

    operator ::taihe::data_holder() const &
    {
        return ::taihe::data_holder(tobj_dup(this->m_handle.data_ptr));
    }

    template<typename Impl>
    struct methods_impl {
        static void *at(struct TVector tobj, uint64_t idx)
        {
            return ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->at(idx);
        }

        static void set(struct TVector tobj, uint64_t idx, void const *val)
        {
            ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->set(idx, val);
        }

        static void insert(struct TVector tobj, uint64_t idx, void const *val)
        {
            ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->insert(idx, val);
        }

        static void push_back(struct TVector tobj, void const *val)
        {
            ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->push_back(val);
        }

        static void pop_back(struct TVector tobj)
        {
            ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->pop_back();
        }

        static void remove(struct TVector tobj, uint64_t idx)
        {
            ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->remove(idx);
        }

        static void clear(struct TVector tobj)
        {
            ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->clear();
        }

        static uint64_t size(struct TVector tobj)
        {
            return ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->size();
        }

        static void fill(struct TVector tobj, void const *val)
        {
            ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->fill(val);
        }

        static uint64_t find(struct TVector tobj, void const *val, uint64_t idx)
        {
            return ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->find(val, idx);
        }

        static struct TIterator iter(struct TVector tobj)
        {
            return ::taihe::into_abi<::taihe::IIterator<value_type>>(
                ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->iter());
        }
    };

    template<typename Impl>
    static constexpr struct TVectorVTable vtbl_impl = {
        .at = &methods_impl<Impl>::at,
        .set = &methods_impl<Impl>::set,
        .insert = &methods_impl<Impl>::insert,
        .push_back = &methods_impl<Impl>::push_back,
        .pop_back = &methods_impl<Impl>::pop_back,
        .remove = &methods_impl<Impl>::remove,
        .clear = &methods_impl<Impl>::clear,
        .size = &methods_impl<Impl>::size,
        .fill = &methods_impl<Impl>::fill,
        .find = &methods_impl<Impl>::find,
        .iter = &methods_impl<Impl>::iter,
    };

    template<typename Impl>
    static constexpr struct IdMapItem idmap_impl[0] = {};

    bool is_error() const &
    {
        return m_handle.vtbl_ptr == nullptr;
    }

    using vtable_type = TVectorVTable;
    using view_type = ::taihe::IVectorView<V>;
    using holder_type = ::taihe::IVector<V>;
    using abi_type = TVector;
};

template<typename V>
struct IVector : ::taihe::IVectorView<V> {
    static constexpr bool is_holder = true;

    explicit IVector(struct TVector handle) : ::taihe::IVectorView<V>(handle)
    {
    }

    IVector &operator=(::taihe::IVector<V> other)
    {
        std::swap(this->m_handle, other.m_handle);
        return *this;
    }

    ~IVector()
    {
        tobj_drop(this->m_handle.data_ptr);
    }

    IVector(::taihe::IVectorView<V> const &other)
        : IVector({
              other.m_handle.vtbl_ptr,
              tobj_dup(other.m_handle.data_ptr),
          })
    {
    }

    IVector(::taihe::IVector<V> const &other)
        : IVector({
              other.m_handle.vtbl_ptr,
              tobj_dup(other.m_handle.data_ptr),
          })
    {
    }

    IVector(::taihe::IVector<V> &&other)
        : IVector({
              other.m_handle.vtbl_ptr,
              std::exchange(other.m_handle.data_ptr, nullptr),
          })
    {
    }

    operator ::taihe::data_view() const &
    {
        return ::taihe::data_view(this->m_handle.data_ptr);
    }

    operator ::taihe::data_holder() const &
    {
        return ::taihe::data_holder(tobj_dup(this->m_handle.data_ptr));
    }

    operator ::taihe::data_holder() &&
    {
        return ::taihe::data_holder(std::exchange(this->m_handle.data_ptr, nullptr));
    }
};

template<typename V>
inline bool operator==(::taihe::IVectorView<V> const &lhs, ::taihe::IVectorView<V> const &rhs)
{
    return ::taihe::data_view(lhs) == ::taihe::data_view(rhs);
}

template<typename V>
inline bool operator==(::taihe::IVector<V> const &lhs, ::taihe::IVector<V> const &rhs)
{
    return ::taihe::data_view(lhs) == ::taihe::data_view(rhs);
}

template<typename V>
struct as_abi<::taihe::IVector<V>> {
    using type = TVector;
};

template<typename V>
struct as_abi<::taihe::IVectorView<V>> {
    using type = TVector;
};

template<typename V>
struct as_param<::taihe::IVector<V>> {
    using type = ::taihe::IVectorView<V>;
};

}  // namespace taihe

#endif  // TAIHE_CONTAINERS_VECTOR_PROJ_HPP
