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

#ifndef TAIHE_CONTAINERS_SET_PROJ_HPP
#define TAIHE_CONTAINERS_SET_PROJ_HPP

#include <taihe/containers/set.abi.h>
#include <taihe/iterator.proj.hpp>

namespace taihe {

template<typename K>
struct ISet;

template<typename K>
struct ISetView {
    using value_type = K;
    static constexpr bool is_holder = false;
    struct TSet m_handle;

    explicit ISetView(struct TSet handle) : m_handle(handle)
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
        static uint8_t insert(struct TSet tobj, void const *val)
        {
            return ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->insert(val);
        }

        static uint8_t remove(struct TSet tobj, void const *val)
        {
            return ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->remove(val);
        }

        static void clear(struct TSet tobj)
        {
            ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->clear();
        }

        static uint8_t contains(struct TSet tobj, void const *val)
        {
            return ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->contains(val);
        }

        static uint64_t size(struct TSet tobj)
        {
            return ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->size();
        }

        static struct TIterator iter(struct TSet tobj)
        {
            return ::taihe::into_abi<::taihe::IIterator<value_type>>(
                ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->iter());
        }
    };

    template<typename Impl>
    static constexpr struct TSetVTable vtbl_impl = {
        .insert = &methods_impl<Impl>::insert,
        .remove = &methods_impl<Impl>::remove,
        .clear = &methods_impl<Impl>::clear,
        .contains = &methods_impl<Impl>::contains,
        .size = &methods_impl<Impl>::size,
        .iter = &methods_impl<Impl>::iter,
    };

    template<typename Impl>
    static constexpr struct IdMapItem idmap_impl[0] = {};

    bool is_error() const &
    {
        return m_handle.vtbl_ptr == nullptr;
    }

    using vtable_type = TSetVTable;
    using view_type = ::taihe::ISetView<K>;
    using holder_type = ::taihe::ISet<K>;
    using abi_type = TSet;
};

template<typename K>
struct ISet : ::taihe::ISetView<K> {
    static constexpr bool is_holder = true;

    explicit ISet(struct TSet handle) : ::taihe::ISetView<K>(handle)
    {
    }

    ISet &operator=(::taihe::ISet<K> other)
    {
        std::swap(this->m_handle, other.m_handle);
        return *this;
    }

    ~ISet()
    {
        tobj_drop(this->m_handle.data_ptr);
    }

    ISet(::taihe::ISetView<K> const &other)
        : ISet({
              other.m_handle.vtbl_ptr,
              tobj_dup(other.m_handle.data_ptr),
          })
    {
    }

    ISet(::taihe::ISet<K> const &other)
        : ISet({
              other.m_handle.vtbl_ptr,
              tobj_dup(other.m_handle.data_ptr),
          })
    {
    }

    ISet(::taihe::ISet<K> &&other)
        : ISet({
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

template<typename K>
inline bool operator==(::taihe::ISetView<K> const &lhs, ::taihe::ISetView<K> const &rhs)
{
    return ::taihe::data_view(lhs) == ::taihe::data_view(rhs);
}

template<typename K>
inline bool operator==(::taihe::ISet<K> const &lhs, ::taihe::ISet<K> const &rhs)
{
    return ::taihe::data_view(lhs) == ::taihe::data_view(rhs);
}

template<typename K>
struct as_abi<::taihe::ISet<K>> {
    using type = TSet;
};

template<typename K>
struct as_abi<::taihe::ISetView<K>> {
    using type = TSet;
};

template<typename K>
struct as_param<::taihe::ISet<K>> {
    using type = ::taihe::ISetView<K>;
};

}  // namespace taihe

#endif  // TAIHE_CONTAINERS_SET_PROJ_HPP
