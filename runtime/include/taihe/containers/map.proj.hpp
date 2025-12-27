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

#ifndef TAIHE_CONTAINERS_MAP_PROJ_HPP
#define TAIHE_CONTAINERS_MAP_PROJ_HPP

#include <taihe/containers/map.abi.h>
#include <taihe/iterator.proj.hpp>

namespace taihe {
template<typename K, typename V>
struct IMap;

template<typename K, typename V>
struct IMapView {
    using value_type = std::pair<K const, V>;
    static constexpr bool is_holder = false;
    struct TMap m_handle;

    explicit IMapView(struct TMap handle) : m_handle(handle)
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
        static void *get(struct TMap tobj, void const *key)
        {
            return ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->get(key);
        }

        static void set(struct TMap tobj, void const *key, void const *val)
        {
            ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->set(key, val);
        }

        static uint8_t insert(struct TMap tobj, void const *key, void const *val)
        {
            return ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->insert(key, val);
        }

        static uint8_t remove(struct TMap tobj, void const *key)
        {
            return ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->remove(key);
        }

        static void clear(struct TMap tobj)
        {
            ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->clear();
        }

        static uint8_t empty(struct TMap tobj)
        {
            return ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->empty();
        }

        static uint8_t contains(struct TMap tobj, void const *key)
        {
            return ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->contains(key);
        }

        static uint64_t size(struct TMap tobj)
        {
            return ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->size();
        }

        static struct TIterator iter(struct TMap tobj)
        {
            return ::taihe::into_abi<::taihe::IIterator<value_type>>(
                ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->iter());
        }
    };

    template<typename Impl>
    static constexpr struct TMapVTable vtbl_impl = {
        .get = &methods_impl<Impl>::get,
        .set = &methods_impl<Impl>::set,
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

    using vtable_type = TMapVTable;
    using view_type = ::taihe::IMapView<K, V>;
    using holder_type = ::taihe::IMap<K, V>;
    using abi_type = TMap;
};

template<typename K, typename V>
struct IMap : ::taihe::IMapView<K, V> {
    static constexpr bool is_holder = true;

    explicit IMap(struct TMap handle) : ::taihe::IMapView<K, V>(handle)
    {
    }

    IMap &operator=(::taihe::IMap<K, V> other)
    {
        std::swap(this->m_handle, other.m_handle);
        return *this;
    }

    ~IMap()
    {
        tobj_drop(this->m_handle.data_ptr);
    }

    IMap(::taihe::IMapView<K, V> const &other)
        : IMap({
              other.m_handle.vtbl_ptr,
              tobj_dup(other.m_handle.data_ptr),
          })
    {
    }

    IMap(::taihe::IMap<K, V> const &other)
        : IMap({
              other.m_handle.vtbl_ptr,
              tobj_dup(other.m_handle.data_ptr),
          })
    {
    }

    IMap(::taihe::IMap<K, V> &&other)
        : IMap({
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

template<typename K, typename V>
inline bool operator==(::taihe::IMapView<K, V> const &lhs, ::taihe::IMapView<K, V> const &rhs)
{
    return ::taihe::data_view(lhs) == ::taihe::data_view(rhs);
}

template<typename K, typename V>
inline bool operator==(::taihe::IMap<K, V> const &lhs, ::taihe::IMap<K, V> const &rhs)
{
    return ::taihe::data_view(lhs) == ::taihe::data_view(rhs);
}

template<typename K, typename V>
struct as_abi<::taihe::IMap<K, V>> {
    using type = TMap;
};

template<typename K, typename V>
struct as_abi<::taihe::IMapView<K, V>> {
    using type = TMap;
};

template<typename K, typename V>
struct as_param<::taihe::IMap<K, V>> {
    using type = ::taihe::IMapView<K, V>;
};
}  // namespace taihe
#endif  // TAIHE_CONTAINERS_MAP_PROJ_HPP