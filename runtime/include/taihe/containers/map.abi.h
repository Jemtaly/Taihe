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

#ifndef TAIHE_CONTAINERS_MAP_ABI_H
#define TAIHE_CONTAINERS_MAP_ABI_H

#include <taihe/iterator.abi.h>
#include <taihe/object.abi.h>

// TMap
// Represents a map structure containing a pointer to the data block
// and a function table pointer.
//
// # Members
// - `vtbl_ptr`: A pointer to the function table associated with map.
// - `data_ptr`: A pointer to the data block.
struct TMap {
    struct TMapVTable const *vtbl_ptr;
    struct DataBlockHead *data_ptr;
};

// TMapVTable
// Function table for map operations.
//
// # Semantics
// - `key` / `val` point to read-only values; the callee does not retain them.
// - Boolean-like results use 0 = false, non-zero = true.
//
// # Members
// - `get`: Lookup value by `key` and return a pointer to the internal
//    value storage. Signals out_of_range if key not found.
// - `set`: Insert or update value for `key`.
// - `insert`: Insert only if `key` does not exist; returns non-zero iff
//    inserted.
// - `remove`: Remove entry for `key`; returns non-zero iff an entry was
//    removed.
// - `clear`: Remove all entries from the map.
// - `contains`: Returns non-zero iff `key` exists in the map.
// - `size`: Return the number of entries in the map.
// - `iter`: Create an iterator over all entries.
struct TMapVTable {
    void *(*get)(struct TMap tobj, void const *key);
    void (*set)(struct TMap tobj, void const *key, void const *val);
    uint8_t (*insert)(struct TMap tobj, void const *key, void const *val);
    uint8_t (*remove)(struct TMap tobj, void const *key);
    void (*clear)(struct TMap tobj);
    uint8_t (*contains)(struct TMap tobj, void const *key);
    uint64_t (*size)(struct TMap tobj);
    struct TIterator (*iter)(struct TMap tobj);
};

#endif  // TAIHE_CONTAINERS_MAP_ABI_H
