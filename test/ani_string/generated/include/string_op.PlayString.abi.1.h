#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "string_op.PlayString.abi.0.h"
struct string_op_PlayString_ftable0;
struct string_op_PlayString_vtable0 {
    struct string_op_PlayString_ftable0 const* ftbl_ptr_0;
};
TH_EXPORT void const* const string_op_PlayString_i0;
struct string_op_PlayString_t0 {
    struct string_op_PlayString_vtable0 const* vtbl_ptr;
    struct DataBlockHead* data_ptr;
};
TH_INLINE struct string_op_PlayString_vtable0 const* string_op_PlayString_dynamic0(struct TypeInfo const* rtti_ptr) {
    for (size_t i = 0; i < rtti_ptr->len; i++) {
        if (rtti_ptr->idmap[i].id == string_op_PlayString_i0) {
            return (struct string_op_PlayString_vtable0 const*)rtti_ptr->idmap[i].vtbl_ptr;
        }
    }
    return NULL;
}
#pragma clang diagnostic pop
