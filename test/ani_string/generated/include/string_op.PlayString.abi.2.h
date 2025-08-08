#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "string_op.PlayString.abi.1.h"
#include "taihe/array.abi.h"
#include "taihe/string.abi.h"
struct string_op_PlayString_ftable0 {
    struct TString (*pickString)(struct string_op_PlayString_t0 tobj, struct TArray nums, int32_t n1, int32_t n2);
    struct TString (*getName)(struct string_op_PlayString_t0 tobj);
    void (*setName)(struct string_op_PlayString_t0 tobj, struct TString name);
};
TH_INLINE struct TString string_op_PlayString_pickString_f0(struct string_op_PlayString_t0 tobj, struct TArray nums, int32_t n1, int32_t n2) {
    return tobj.vtbl_ptr->ftbl_ptr_0->pickString(tobj, nums, n1, n2);
}
TH_INLINE struct TString string_op_PlayString_getName_f0(struct string_op_PlayString_t0 tobj) {
    return tobj.vtbl_ptr->ftbl_ptr_0->getName(tobj);
}
TH_INLINE void string_op_PlayString_setName_f0(struct string_op_PlayString_t0 tobj, struct TString name) {
    return tobj.vtbl_ptr->ftbl_ptr_0->setName(tobj, name);
}
#pragma clang diagnostic pop
