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

#ifndef TAIHE_ITERATOR_ABI_H
#define TAIHE_ITERATOR_ABI_H

#include <taihe/object.abi.h>

// TIterator
// Represents an iterator object containing a pointer to the data block
// and a function table pointer.
//
// # Members
// - `vtbl_ptr`: A pointer to the function table associated with iterator.
// - `data_ptr`: A pointer to the data block.
struct TIterator {
    struct TIteratorVTable const *vtbl_ptr;
    struct DataBlockHead *data_ptr;
};

// TIteratorVTable
// Function table for iterator operations.
//
// # Members
// - `get`: Return a pointer to the current element.
// - `move_next`: Advance the iterator to the next element.
// - `is_end`: Check whether the iterator is at end; returns non-zero if at end.
struct TIteratorVTable {
    void const *(*get)(struct TIterator tobj);
    void (*move_next)(struct TIterator tobj);
    uint8_t (*is_end)(struct TIterator tobj);
};

#endif  // TAIHE_ITERATOR_ABI_H