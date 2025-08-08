#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "string_op.StringPair.ani.0.hpp"
#include "string_op.StringPair.proj.2.hpp"
inline ::string_op::StringPair taihe::from_ani_t<::string_op::StringPair>::operator()(ani_env* env, ani_object ani_obj) const {
    ani_string ani_field__0 = {};
    env->Object_CallMethod_Ref(ani_obj, TH_ANI_FIND_CLASS_METHOD(env, "string_op.StringPair", "<get>_0", nullptr), reinterpret_cast<ani_ref*>(&ani_field__0));
    ani_size cpp_field__0_len = {};
    env->String_GetUTF8Size(ani_field__0, &cpp_field__0_len);
    TString cpp_field__0_tstr;
    char* cpp_field__0_buf = tstr_initialize(&cpp_field__0_tstr, cpp_field__0_len + 1);
    env->String_GetUTF8(ani_field__0, cpp_field__0_buf, cpp_field__0_len + 1, &cpp_field__0_len);
    cpp_field__0_buf[cpp_field__0_len] = '\0';
    cpp_field__0_tstr.length = cpp_field__0_len;
    ::taihe::string cpp_field__0 = ::taihe::string(cpp_field__0_tstr);
    ani_string ani_field__1 = {};
    env->Object_CallMethod_Ref(ani_obj, TH_ANI_FIND_CLASS_METHOD(env, "string_op.StringPair", "<get>_1", nullptr), reinterpret_cast<ani_ref*>(&ani_field__1));
    ani_size cpp_field__1_len = {};
    env->String_GetUTF8Size(ani_field__1, &cpp_field__1_len);
    TString cpp_field__1_tstr;
    char* cpp_field__1_buf = tstr_initialize(&cpp_field__1_tstr, cpp_field__1_len + 1);
    env->String_GetUTF8(ani_field__1, cpp_field__1_buf, cpp_field__1_len + 1, &cpp_field__1_len);
    cpp_field__1_buf[cpp_field__1_len] = '\0';
    cpp_field__1_tstr.length = cpp_field__1_len;
    ::taihe::string cpp_field__1 = ::taihe::string(cpp_field__1_tstr);
    return ::string_op::StringPair{std::move(cpp_field__0), std::move(cpp_field__1)};
}
inline ani_object taihe::into_ani_t<::string_op::StringPair>::operator()(ani_env* env, ::string_op::StringPair cpp_obj) const {
    ani_string ani_field__0 = {};
    env->String_NewUTF8(cpp_obj._0.c_str(), cpp_obj._0.size(), &ani_field__0);
    ani_string ani_field__1 = {};
    env->String_NewUTF8(cpp_obj._1.c_str(), cpp_obj._1.size(), &ani_field__1);
    ani_object ani_obj = {};
    env->Object_New(TH_ANI_FIND_CLASS(env, "string_op._taihe_StringPair_inner"), TH_ANI_FIND_CLASS_METHOD(env, "string_op._taihe_StringPair_inner", "<ctor>", nullptr), &ani_obj, ani_field__0, ani_field__1);
    return ani_obj;
}
#pragma clang diagnostic pop
