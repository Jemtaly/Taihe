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
}

template<typename T>
TVector<T> tvec_resize(TVector<T>* handle, std::size_t cap) {
    size_t bytes_required = sizeof(TVector<T>) + sizeof(T) * cap;
    return reinterpret_cast<TVector<T>*>(realloc(handle, cap));
}

namespace taihe::core {}
