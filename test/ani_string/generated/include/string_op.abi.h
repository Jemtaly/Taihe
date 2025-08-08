#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "string_op.StringPair.abi.2.h"
#include "string_op.PlayString.abi.2.h"
#include "taihe/common.h"
#include "taihe/string.abi.h"
#include "taihe/array.abi.h"
TH_EXPORT struct TString string_op_concatString_f0(struct TString a, struct TString b);
TH_EXPORT struct TString string_op_makeString_f0(struct TString a, int32_t b);
TH_EXPORT struct string_op_StringPair_t0 string_op_split_f0(struct TString a, int32_t n);
TH_EXPORT struct TArray string_op_split2_f0(struct TString a, int32_t n);
TH_EXPORT int32_t string_op_to_i32_f02(struct TString a);
TH_EXPORT struct TString string_op_from_i32_f02(int32_t a);
TH_EXPORT struct string_op_PlayString_t0 string_op_makePlayStringIface_f0();
TH_EXPORT float string_op_to_f32_f02(struct TString a);
TH_EXPORT struct TString string_op_from_f32_f02(float a);
TH_EXPORT struct TString string_op_concatString2_f0(struct TString s, int32_t n, struct TArray sArr, bool b, struct TArray buffer);
#pragma clang diagnostic pop
