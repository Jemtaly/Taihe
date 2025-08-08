#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "string_op.PlayString.ani.0.hpp"
#include "string_op.PlayString.proj.2.hpp"
inline ::string_op::PlayString taihe::from_ani_t<::string_op::PlayString>::operator()(ani_env* env, ani_object ani_obj) const {
    struct cpp_impl_t : ::taihe::dref_guard {
        cpp_impl_t(ani_env* env, ani_ref val) : ::taihe::dref_guard(env, val) {}
        ::taihe::string pickString(::taihe::array_view<::taihe::string> cpp_arg_nums, int32_t cpp_arg_n1, int32_t cpp_arg_n2) {
            ::taihe::env_guard guard;
            ani_env *env = guard.get_env();
            size_t ani_arg_nums_size = cpp_arg_nums.size();
            ani_array ani_arg_nums = {};
            ani_ref ani_arg_nums_undef = {};
            env->GetUndefined(&ani_arg_nums_undef);
            env->Array_New(ani_arg_nums_size, ani_arg_nums_undef, &ani_arg_nums);
            for (size_t ani_arg_nums_i = 0; ani_arg_nums_i < ani_arg_nums_size; ani_arg_nums_i++) {
                ani_string ani_arg_nums_item = {};
                env->String_NewUTF8(cpp_arg_nums[ani_arg_nums_i].c_str(), cpp_arg_nums[ani_arg_nums_i].size(), &ani_arg_nums_item);
                env->Array_Set(ani_arg_nums, ani_arg_nums_i, ani_arg_nums_item);
            }
            ani_int ani_arg_n1 = static_cast<ani_int>(cpp_arg_n1);
            ani_int ani_arg_n2 = static_cast<ani_int>(cpp_arg_n2);
            ani_string ani_result = {};
            env->Function_Call_Ref(TH_ANI_FIND_MODULE_FUNCTION(env, "string_op", "_taihe_PlayString_pickString_reverse", nullptr), reinterpret_cast<ani_ref*>(&ani_result), static_cast<ani_object>(this->ref), ani_arg_nums, ani_arg_n1, ani_arg_n2);
            ani_size cpp_result_len = {};
            env->String_GetUTF8Size(ani_result, &cpp_result_len);
            TString cpp_result_tstr;
            char* cpp_result_buf = tstr_initialize(&cpp_result_tstr, cpp_result_len + 1);
            env->String_GetUTF8(ani_result, cpp_result_buf, cpp_result_len + 1, &cpp_result_len);
            cpp_result_buf[cpp_result_len] = '\0';
            cpp_result_tstr.length = cpp_result_len;
            ::taihe::string cpp_result = ::taihe::string(cpp_result_tstr);
            return cpp_result;
        }
        ::taihe::string getName() {
            ::taihe::env_guard guard;
            ani_env *env = guard.get_env();
            ani_string ani_result = {};
            env->Function_Call_Ref(TH_ANI_FIND_MODULE_FUNCTION(env, "string_op", "_taihe_PlayString_getName_reverse", nullptr), reinterpret_cast<ani_ref*>(&ani_result), static_cast<ani_object>(this->ref));
            ani_size cpp_result_len = {};
            env->String_GetUTF8Size(ani_result, &cpp_result_len);
            TString cpp_result_tstr;
            char* cpp_result_buf = tstr_initialize(&cpp_result_tstr, cpp_result_len + 1);
            env->String_GetUTF8(ani_result, cpp_result_buf, cpp_result_len + 1, &cpp_result_len);
            cpp_result_buf[cpp_result_len] = '\0';
            cpp_result_tstr.length = cpp_result_len;
            ::taihe::string cpp_result = ::taihe::string(cpp_result_tstr);
            return cpp_result;
        }
        void setName(::taihe::string_view cpp_arg_name) {
            ::taihe::env_guard guard;
            ani_env *env = guard.get_env();
            ani_string ani_arg_name = {};
            env->String_NewUTF8(cpp_arg_name.c_str(), cpp_arg_name.size(), &ani_arg_name);
            env->Function_Call_Void(TH_ANI_FIND_MODULE_FUNCTION(env, "string_op", "_taihe_PlayString_setName_reverse", nullptr), static_cast<ani_object>(this->ref), ani_arg_name);
        }
        uintptr_t getGlobalReference() const {
            return reinterpret_cast<uintptr_t>(this->ref);
        }
    };
    return ::taihe::make_holder<cpp_impl_t, ::string_op::PlayString, ::taihe::platform::ani::AniObject>(env, ani_obj);
}
inline ani_object taihe::into_ani_t<::string_op::PlayString>::operator()(ani_env* env, ::string_op::PlayString cpp_obj) const {
    ani_long ani_vtbl_ptr = reinterpret_cast<ani_long>(cpp_obj.m_handle.vtbl_ptr);
    ani_long ani_data_ptr = reinterpret_cast<ani_long>(cpp_obj.m_handle.data_ptr);
    cpp_obj.m_handle.data_ptr = nullptr;
    ani_object ani_obj;
    env->Object_New(TH_ANI_FIND_CLASS(env, "string_op._taihe_PlayString_inner"), TH_ANI_FIND_CLASS_METHOD(env, "string_op._taihe_PlayString_inner", "_taihe_initialize", "ll:"), &ani_obj, ani_vtbl_ptr, ani_data_ptr);
    return ani_obj;
}
#pragma clang diagnostic pop
