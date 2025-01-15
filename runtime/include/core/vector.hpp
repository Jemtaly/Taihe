#pragma once

#include <vector>
#include <cstddef>
#include <cstdlib>

#include <taihe/common.hpp>

template<typename T>
struct TVector {
    TRefCount count;
    std::size_t len;
    std::size_t cap;
    T data[];
};

template<typename T>
TVector<T>* tvec_dup(TVector<T>* handle) {
    if (handle) {
        tref_inc(&handle->count);
    }
    return handle;
}

template<typename T>
void tvec_drop(TVector<T>* handle) {
    if (handle && tref_dec(&handle->count)) {
        for (std::size_t i = 0; i < handle->len; i++) {
            std::destroy_at(&handle->data[i]);
        }
        free(handle);
    }
}

template<typename T>
TVector<T> tvec_new(std::size_t cap) {
    size_t bytes_required = sizeof(TVector<T>) + sizeof(T) * cap;
    TVector<T>* handle = reinterpret_cast<TVector<T>*>(malloc(bytes_required));
    tref_set(&handle->count, 1);
    handle->len = 0;
    handle->cap = cap;
    return handle;
}

template<typename T>
TVector<T>* tvec_resize(TVector<T>* handle, std::size_t cap) {
    size_t bytes_required = sizeof(TVector<T>) + sizeof(T) * cap;
    handle = reinterpret_cast<TVector<T>*>(realloc(handle, bytes_required));
    handle->cap = cap;
    return handle;
}

namespace taihe::core {

template<typename T>
struct vector {
public:
    using value_type = T;
    using size_type = std::size_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using iterator = T*;
    using const_iterator = const T*;

    vector() : m_data(tvec_new<T>(0)) {}

    explicit vector(size_type cap) 
        : m_data(tvec_new<T>(cap)) {}

    vector(const T* array, std::size_t size)
        : m_data(tvec_new<T>(size)) {
        for (std::size_t i = 0; i < size; ++i) {
            new (&m_data->data[i]) T(array[i]);
        }
        m_data->len = size;
    }

    vector(std::size_t size, const T& value)
        : m_data(tvec_new<T>(size)) {
        for (std::size_t i = 0; i < size; ++i) {
            new (&m_data->data[i]) T(value);
        }
        m_data->len = size;
    }

    template <typename InputIt>
    vector(InputIt first, InputIt last) {
        auto size = std::distance(first, last);
        m_data = tvec_new<T>(size);
        std::size_t index = 0;
        for (auto it = first; it != last; ++it) {
            new (&m_data->data[index++]) T(*it);
        }
        m_data->len = size;
    }

    vector(std::initializer_list<value_type> value)
        : vector(value.begin(), value.end()) {}

    ~vector() {
        tvec_drop(m_data);
    }

    vector(const vector& other)
        : m_data(tvec_dup(other.m_data)) {}

    vector(vector&& other) noexcept
        : m_data(other.m_data) {
        other.m_data = nullptr;
    }

    vector& operator=(const vector& other) {
        if (this != &other) {
            tvec_drop(m_data);
            m_data = tvec_dup(other.m_data);
        }
        return *this;
    }

    vector& operator=(vector&& other) noexcept {
        if (this != &other) {
            tvec_drop(m_data);
            m_data = other.m_data;
            other.m_data = nullptr;
        }
        return *this;
    }

    std::size_t size() const noexcept {
        return m_data->len;
    }

    std::size_t capacity() const noexcept {
        return m_data->cap;
    }

    void push_back(T&& value) {
        ensure_capacity(m_data->len + 1);
        new (&m_data->data[m_data->len]) T(std::move(value));
        ++m_data->len;
    }

    void push_back(T const& value) {
        ensure_capacity(m_data->len + 1);
        new (&m_data->data[m_data->len]) T(value);
        ++m_data->len;
    }

    template <typename... Args>
    T& emplace_back(Args&&... args) {
        ensure_capacity(m_data->len + 1);
        T* location = &m_data->data[m_data->len];
        new (location) T(std::forward<Args>(args)...);
        ++m_data->len;
        return *location;
    }

    void pop_back() {
        if (m_data->len > 0) {
            --m_data->len;
            std::destroy_at(&m_data->data[m_data->len]);
        }
    }

    void clear() noexcept {
        for (std::size_t i = 0; i < m_data->len; i++) {
            std::destroy_at(&m_data->data[i]);
        }
        m_data->len = 0;
    }

    T& operator[](std::size_t index) {
        return m_data->data[index];
    }

    const T& operator[](std::size_t index) const {
        return m_data->data[index];
    }

private:
    TVector<T>* m_data;

    void ensure_capacity(std::size_t requird_cap) {
        if (requird_cap > m_data->cap) {
            std::size_t new_capacity = std::max(requird_cap, m_data->cap * 2);
            m_data = tvec_resize(m_data, new_capacity);
        }
    }
};

}
