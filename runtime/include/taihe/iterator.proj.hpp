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

#ifndef TAIHE_ITERATOR_PROJ_HPP
#define TAIHE_ITERATOR_PROJ_HPP

#include <taihe/iterator.abi.h>
#include <taihe/object.hpp>

namespace taihe {
template<typename T>
struct IIterator;

template<typename T>
struct IIteratorView {
    static constexpr bool is_holder = false;
    struct TIterator m_handle;

    explicit IIteratorView(struct TIterator handle) : m_handle(handle)
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
    struct method_impl {
        static void const *get(struct TIterator tobj)
        {
            return ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->get();
        }

        static void move_next(struct TIterator tobj)
        {
            ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->move_next();
        }

        static uint8_t is_end(struct TIterator tobj)
        {
            return ::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->is_end();
        }
    };

    template<typename Impl>
    static constexpr TIteratorVTable vtbl_impl = {
        .get = &method_impl<Impl>::get,
        .move_next = &method_impl<Impl>::move_next,
        .is_end = &method_impl<Impl>::is_end,
    };

    template<typename Impl>
    static constexpr struct IdMapItem idmap_impl[0] = {};

    bool is_error() const &
    {
        return m_handle.vtbl_ptr == nullptr;
    }

    using view_type = ::taihe::IIteratorView<T>;
    using holder_type = ::taihe::IIterator<T>;
    using vtable_type = TIteratorVTable;
    using abi_type = TIterator;
};

template<typename T>
struct IIterator : public ::taihe::IIteratorView<T> {
    static constexpr bool is_holder = true;

    explicit IIterator(struct TIterator handle) : ::taihe::IIteratorView<T>(handle)
    {
    }

    IIterator &operator=(::taihe::IIterator<T> other)
    {
        ::std::swap(this->m_handle, other.m_handle);
        return *this;
    }

    ~IIterator()
    {
        tobj_drop(this->m_handle.data_ptr);
    }

    IIterator(::taihe::IIteratorView<T> const &other)
        : IIterator({
              other.m_handle.vtbl_ptr,
              tobj_dup(other.m_handle.data_ptr),
          })
    {
    }

    IIterator(::taihe::IIterator<T> const &other)
        : IIterator({
              other.m_handle.vtbl_ptr,
              tobj_dup(other.m_handle.data_ptr),
          })
    {
    }

    IIterator(::taihe::IIterator<T> &&other)
        : IIterator({
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

template<typename T>
inline bool operator==(::taihe::IIteratorView<T> const &lhs, ::taihe::IIteratorView<T> const &rhs)
{
    return ::taihe::data_view(lhs) == ::taihe::data_view(rhs);
}

template<typename T>
inline bool operator==(::taihe::IIterator<T> const &lhs, ::taihe::IIterator<T> const &rhs)
{
    return ::taihe::data_view(lhs) == ::taihe::data_view(rhs);
}

template<typename T>
struct as_abi<::taihe::IIterator<T>> {
    using type = TIterator;
};

template<typename T>
struct as_abi<::taihe::IIteratorView<T>> {
    using type = TIterator;
};

template<typename T>
struct as_param<::taihe::IIterator<T>> {
    using type = ::taihe::IIteratorView<T>;
};
}  // namespace taihe
#endif  // TAIHE_ITERATOR_PROJ_HPP