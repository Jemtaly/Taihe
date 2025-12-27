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

#ifndef TAIHE_CONTAINERS_VECTOR_ABI_H
#define TAIHE_CONTAINERS_VECTOR_ABI_H

#include <taihe/iterator.abi.h>
#include <taihe/object.abi.h>

#include <stdint.h>

// TVector
// Represents a vector structure containing a pointer to the data block
// and a function table pointer.
//
// # Members
// - `vtbl_ptr`: A pointer to the function table associated with vector.
// - `data_ptr`: A pointer to the data block.
struct TVector {
    struct TVectorVTable const *vtbl_ptr;
    struct DataBlockHead *data_ptr;
};

// TVectorVTable
// Function table for vector operations.
//
// Notes:
// - index uses uint64_t for ABI stability across languages.
//
// #Members
// - `at`: Access element at index; throws out_of_range if invalid.
// - `set`: Assign value at index; throws out_of_range if invalid.
// - `insert`: Insert value at index; throws out_of_range if invalid.
// - `push_back`: Append value to the end of the vector.
// - `pop_back`: Remove last element; throws out_of_range if empty.
// - `remove`: Erase element at index; throws out_of_range if invalid.
// - `clear`: Remove all elements from the vector.
// - `size`: Return number of elements in the vector.
// - `fill`: Set all existing elements to the given value.
// - `find`: Locate first occurrence of value from index; returns
//    npos(UINT64_MAX) if not found.
// - `iter`: Create an iterator over all elements.
struct TVectorVTable {
    void *(*at)(struct TVector tobj, uint64_t idx);
    void (*set)(struct TVector tobj, uint64_t idx, void const *val);
    void (*insert)(struct TVector tobj, uint64_t idx, void const *val);
    void (*push_back)(struct TVector tobj, void const *val);
    void (*pop_back)(struct TVector tobj);
    void (*remove)(struct TVector tobj, uint64_t idx);
    void (*clear)(struct TVector tobj);
    uint64_t (*size)(struct TVector tobj);
    void (*fill)(struct TVector tobj, void const *val);
    uint64_t (*find)(struct TVector tobj, void const *val, uint64_t idx);
    struct TIterator (*iter)(struct TVector tobj);
};

#endif  // TAIHE_CONTAINERS_VECTOR_ABI_H
