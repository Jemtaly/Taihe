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

#ifndef TAIHE_CONTAINERS_SET_ABI_H
#define TAIHE_CONTAINERS_SET_ABI_H

#include <taihe/iterator.abi.h>
#include <taihe/object.abi.h>

// TSet
// Represents a set structure containing a pointer to the data block
// and a function table pointer.
//
// # Members
// - `vtbl_ptr`: A pointer to the function table associated with set.
// - `data_ptr`: A pointer to the data block.
struct TSet {
    struct TSetVTable const *vtbl_ptr;
    struct DataBlockHead *data_ptr;
};

// TSetVTable
// Function table for set operations.
//
// # Semantics
// - `key` points to a read-only key value; the callee does not retain `key`.
// - Boolean-like results use 0 = false, non-zero = true.
//
// # Members
// - `insert`: Insert `key`; returns non-zero iff the key was newly inserted.
// - `remove`: Remove `key`; returns non-zero iff an existing key was removed.
// - `clear`: Remove all keys from the set.
// - `contains`: Returns non-zero iff `key` exists in the set.
// - `size`: Return the number of keys in the set.
// - `iter`: Create an iterator over keys.
struct TSetVTable {
    uint8_t (*insert)(struct TSet tobj, void const *key);
    uint8_t (*remove)(struct TSet tobj, void const *key);
    void (*clear)(struct TSet tobj);
    uint8_t (*contains)(struct TSet tobj, void const *key);
    uint64_t (*size)(struct TSet tobj);
    struct TIterator (*iter)(struct TSet tobj);
};

#endif  // TAIHE_CONTAINERS_SET_ABI_H
