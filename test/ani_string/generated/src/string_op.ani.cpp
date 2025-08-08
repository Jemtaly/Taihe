#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe/object.hpp"
#include "string_op.ani.hpp"
#include "string_op.user.hpp"
#include "string_op.StringPair.ani.1.hpp"
#include "string_op.PlayString.ani.1.hpp"
namespace local {
static void _obj_drop([[maybe_unused]] ani_env *env, ani_long data_ptr) {
    tobj_drop(reinterpret_cast<DataBlockHead*>(data_ptr));
}
static ani_long _obj_dup([[maybe_unused]] ani_env *env, ani_long data_ptr) {
    return reinterpret_cast<ani_long>(tobj_dup(reinterpret_cast<DataBlockHead*>(data_ptr)));
}
static ani_ref _native_invoke([[maybe_unused]] ani_env *env, ani_long ani_cast_ptr, ani_long ani_func_ptr, ani_long ani_data_ptr, ani_ref arg_0, ani_ref arg_1, ani_ref arg_2, ani_ref arg_3, ani_ref arg_4, ani_ref arg_5, ani_ref arg_6, ani_ref arg_7, ani_ref arg_8, ani_ref arg_9, ani_ref arg_10, ani_ref arg_11, ani_ref arg_12, ani_ref arg_13, ani_ref arg_14, ani_ref arg_15) {
    return reinterpret_cast<ani_ref (*)(ani_env *env, ani_long ani_func_ptr, ani_long ani_data_ptr, ani_ref arg_0, ani_ref arg_1, ani_ref arg_2, ani_ref arg_3, ani_ref arg_4, ani_ref arg_5, ani_ref arg_6, ani_ref arg_7, ani_ref arg_8, ani_ref arg_9, ani_ref arg_10, ani_ref arg_11, ani_ref arg_12, ani_ref arg_13, ani_ref arg_14, ani_ref arg_15)>(ani_cast_ptr)(env, ani_func_ptr, ani_data_ptr, arg_0, arg_1, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, arg_8, arg_9, arg_10, arg_11, arg_12, arg_13, arg_14, arg_15);
}
static ani_status ANIUtilsRegister(ani_env *env) {
    ani_module scope;
    if (ANI_OK != env->FindModule("string_op", &scope)) {
        return ANI_ERROR;
    }
    ani_native_function methods[] = {
        {"_taihe_objDrop", nullptr, reinterpret_cast<void*>(local::_obj_drop)},
        {"_taihe_objDup", nullptr, reinterpret_cast<void*>(local::_obj_dup)},
        {"_taihe_nativeInvoke", nullptr, reinterpret_cast<void*>(local::_native_invoke)},
    };
    return env->Module_BindNativeFunctions(scope, methods, sizeof(methods) / sizeof(ani_native_function));
}
static ani_string concatString([[maybe_unused]] ani_env *env, ani_string ani_arg_a, ani_string ani_arg_b) {
    ani_size cpp_arg_a_len = {};
    env->String_GetUTF8Size(ani_arg_a, &cpp_arg_a_len);
    TString cpp_arg_a_tstr;
    char* cpp_arg_a_buf = tstr_initialize(&cpp_arg_a_tstr, cpp_arg_a_len + 1);
    env->String_GetUTF8(ani_arg_a, cpp_arg_a_buf, cpp_arg_a_len + 1, &cpp_arg_a_len);
    cpp_arg_a_buf[cpp_arg_a_len] = '\0';
    cpp_arg_a_tstr.length = cpp_arg_a_len;
    ::taihe::string cpp_arg_a = ::taihe::string(cpp_arg_a_tstr);
    ani_size cpp_arg_b_len = {};
    env->String_GetUTF8Size(ani_arg_b, &cpp_arg_b_len);
    TString cpp_arg_b_tstr;
    char* cpp_arg_b_buf = tstr_initialize(&cpp_arg_b_tstr, cpp_arg_b_len + 1);
    env->String_GetUTF8(ani_arg_b, cpp_arg_b_buf, cpp_arg_b_len + 1, &cpp_arg_b_len);
    cpp_arg_b_buf[cpp_arg_b_len] = '\0';
    cpp_arg_b_tstr.length = cpp_arg_b_len;
    ::taihe::string cpp_arg_b = ::taihe::string(cpp_arg_b_tstr);
    ::taihe::string cpp_result = ::string_op::concatString(cpp_arg_a, cpp_arg_b);
    if (::taihe::has_error()) { return ani_string{}; }
    ani_string ani_result = {};
    env->String_NewUTF8(cpp_result.c_str(), cpp_result.size(), &ani_result);
    return ani_result;
}
static ani_string makeString([[maybe_unused]] ani_env *env, ani_string ani_arg_a, ani_int ani_arg_b) {
    ani_size cpp_arg_a_len = {};
    env->String_GetUTF8Size(ani_arg_a, &cpp_arg_a_len);
    TString cpp_arg_a_tstr;
    char* cpp_arg_a_buf = tstr_initialize(&cpp_arg_a_tstr, cpp_arg_a_len + 1);
    env->String_GetUTF8(ani_arg_a, cpp_arg_a_buf, cpp_arg_a_len + 1, &cpp_arg_a_len);
    cpp_arg_a_buf[cpp_arg_a_len] = '\0';
    cpp_arg_a_tstr.length = cpp_arg_a_len;
    ::taihe::string cpp_arg_a = ::taihe::string(cpp_arg_a_tstr);
    int32_t cpp_arg_b = static_cast<int32_t>(ani_arg_b);
    ::taihe::string cpp_result = ::string_op::makeString(cpp_arg_a, cpp_arg_b);
    if (::taihe::has_error()) { return ani_string{}; }
    ani_string ani_result = {};
    env->String_NewUTF8(cpp_result.c_str(), cpp_result.size(), &ani_result);
    return ani_result;
}
static ani_object split([[maybe_unused]] ani_env *env, ani_string ani_arg_a, ani_int ani_arg_n) {
    ani_size cpp_arg_a_len = {};
    env->String_GetUTF8Size(ani_arg_a, &cpp_arg_a_len);
    TString cpp_arg_a_tstr;
    char* cpp_arg_a_buf = tstr_initialize(&cpp_arg_a_tstr, cpp_arg_a_len + 1);
    env->String_GetUTF8(ani_arg_a, cpp_arg_a_buf, cpp_arg_a_len + 1, &cpp_arg_a_len);
    cpp_arg_a_buf[cpp_arg_a_len] = '\0';
    cpp_arg_a_tstr.length = cpp_arg_a_len;
    ::taihe::string cpp_arg_a = ::taihe::string(cpp_arg_a_tstr);
    int32_t cpp_arg_n = static_cast<int32_t>(ani_arg_n);
    ::string_op::StringPair cpp_result = ::string_op::split(cpp_arg_a, cpp_arg_n);
    if (::taihe::has_error()) { return ani_object{}; }
    ani_object ani_result = ::taihe::into_ani<::string_op::StringPair>(env, cpp_result);
    return ani_result;
}
static ani_array split2([[maybe_unused]] ani_env *env, ani_string ani_arg_a, ani_int ani_arg_n) {
    ani_size cpp_arg_a_len = {};
    env->String_GetUTF8Size(ani_arg_a, &cpp_arg_a_len);
    TString cpp_arg_a_tstr;
    char* cpp_arg_a_buf = tstr_initialize(&cpp_arg_a_tstr, cpp_arg_a_len + 1);
    env->String_GetUTF8(ani_arg_a, cpp_arg_a_buf, cpp_arg_a_len + 1, &cpp_arg_a_len);
    cpp_arg_a_buf[cpp_arg_a_len] = '\0';
    cpp_arg_a_tstr.length = cpp_arg_a_len;
    ::taihe::string cpp_arg_a = ::taihe::string(cpp_arg_a_tstr);
    int32_t cpp_arg_n = static_cast<int32_t>(ani_arg_n);
    ::taihe::array<::taihe::string> cpp_result = ::string_op::split2(cpp_arg_a, cpp_arg_n);
    if (::taihe::has_error()) { return ani_array{}; }
    size_t ani_result_size = cpp_result.size();
    ani_array ani_result = {};
    ani_ref ani_result_undef = {};
    env->GetUndefined(&ani_result_undef);
    env->Array_New(ani_result_size, ani_result_undef, &ani_result);
    for (size_t ani_result_i = 0; ani_result_i < ani_result_size; ani_result_i++) {
        ani_string ani_result_item = {};
        env->String_NewUTF8(cpp_result[ani_result_i].c_str(), cpp_result[ani_result_i].size(), &ani_result_item);
        env->Array_Set(ani_result, ani_result_i, ani_result_item);
    }
    return ani_result;
}
static ani_int to_i32([[maybe_unused]] ani_env *env, ani_string ani_arg_a) {
    ani_size cpp_arg_a_len = {};
    env->String_GetUTF8Size(ani_arg_a, &cpp_arg_a_len);
    TString cpp_arg_a_tstr;
    char* cpp_arg_a_buf = tstr_initialize(&cpp_arg_a_tstr, cpp_arg_a_len + 1);
    env->String_GetUTF8(ani_arg_a, cpp_arg_a_buf, cpp_arg_a_len + 1, &cpp_arg_a_len);
    cpp_arg_a_buf[cpp_arg_a_len] = '\0';
    cpp_arg_a_tstr.length = cpp_arg_a_len;
    ::taihe::string cpp_arg_a = ::taihe::string(cpp_arg_a_tstr);
    int32_t cpp_result = ::string_op::to_i32(cpp_arg_a);
    if (::taihe::has_error()) { return ani_int{}; }
    ani_int ani_result = static_cast<ani_int>(cpp_result);
    return ani_result;
}
static ani_string from_i32([[maybe_unused]] ani_env *env, ani_int ani_arg_a) {
    int32_t cpp_arg_a = static_cast<int32_t>(ani_arg_a);
    ::taihe::string cpp_result = ::string_op::from_i32(cpp_arg_a);
    if (::taihe::has_error()) { return ani_string{}; }
    ani_string ani_result = {};
    env->String_NewUTF8(cpp_result.c_str(), cpp_result.size(), &ani_result);
    return ani_result;
}
static ani_object makePlayStringIface([[maybe_unused]] ani_env *env) {
    ::string_op::PlayString cpp_result = ::string_op::makePlayStringIface();
    if (::taihe::has_error()) { return ani_object{}; }
    ani_object ani_result = ::taihe::into_ani<::string_op::PlayString>(env, cpp_result);
    return ani_result;
}
static ani_float to_f32([[maybe_unused]] ani_env *env, ani_string ani_arg_a) {
    ani_size cpp_arg_a_len = {};
    env->String_GetUTF8Size(ani_arg_a, &cpp_arg_a_len);
    TString cpp_arg_a_tstr;
    char* cpp_arg_a_buf = tstr_initialize(&cpp_arg_a_tstr, cpp_arg_a_len + 1);
    env->String_GetUTF8(ani_arg_a, cpp_arg_a_buf, cpp_arg_a_len + 1, &cpp_arg_a_len);
    cpp_arg_a_buf[cpp_arg_a_len] = '\0';
    cpp_arg_a_tstr.length = cpp_arg_a_len;
    ::taihe::string cpp_arg_a = ::taihe::string(cpp_arg_a_tstr);
    float cpp_result = ::string_op::to_f32(cpp_arg_a);
    if (::taihe::has_error()) { return ani_float{}; }
    ani_float ani_result = static_cast<ani_float>(cpp_result);
    return ani_result;
}
static ani_string from_f32([[maybe_unused]] ani_env *env, ani_float ani_arg_a) {
    float cpp_arg_a = static_cast<float>(ani_arg_a);
    ::taihe::string cpp_result = ::string_op::from_f32(cpp_arg_a);
    if (::taihe::has_error()) { return ani_string{}; }
    ani_string ani_result = {};
    env->String_NewUTF8(cpp_result.c_str(), cpp_result.size(), &ani_result);
    return ani_result;
}
static ani_string concatString2([[maybe_unused]] ani_env *env, ani_string ani_arg_s, ani_int ani_arg_n, ani_array ani_arg_sArr, ani_boolean ani_arg_b, ani_arraybuffer ani_arg_buffer) {
    ani_size cpp_arg_s_len = {};
    env->String_GetUTF8Size(ani_arg_s, &cpp_arg_s_len);
    TString cpp_arg_s_tstr;
    char* cpp_arg_s_buf = tstr_initialize(&cpp_arg_s_tstr, cpp_arg_s_len + 1);
    env->String_GetUTF8(ani_arg_s, cpp_arg_s_buf, cpp_arg_s_len + 1, &cpp_arg_s_len);
    cpp_arg_s_buf[cpp_arg_s_len] = '\0';
    cpp_arg_s_tstr.length = cpp_arg_s_len;
    ::taihe::string cpp_arg_s = ::taihe::string(cpp_arg_s_tstr);
    int32_t cpp_arg_n = static_cast<int32_t>(ani_arg_n);
    ani_size cpp_arg_sArr_size = {};
    env->Array_GetLength(ani_arg_sArr, &cpp_arg_sArr_size);
    ::taihe::string* cpp_arg_sArr_buffer = reinterpret_cast<::taihe::string*>(malloc(cpp_arg_sArr_size * sizeof(::taihe::string)));
    for (size_t cpp_arg_sArr_buffer_i = 0; cpp_arg_sArr_buffer_i < cpp_arg_sArr_size; cpp_arg_sArr_buffer_i++) {
        ani_ref cpp_arg_sArr_buffer_ani_item = {};
        env->Array_Get(ani_arg_sArr, cpp_arg_sArr_buffer_i, &cpp_arg_sArr_buffer_ani_item);
        ani_size cpp_arg_sArr_buffer_cpp_item_len = {};
        env->String_GetUTF8Size(static_cast<ani_string>(cpp_arg_sArr_buffer_ani_item), &cpp_arg_sArr_buffer_cpp_item_len);
        TString cpp_arg_sArr_buffer_cpp_item_tstr;
        char* cpp_arg_sArr_buffer_cpp_item_buf = tstr_initialize(&cpp_arg_sArr_buffer_cpp_item_tstr, cpp_arg_sArr_buffer_cpp_item_len + 1);
        env->String_GetUTF8(static_cast<ani_string>(cpp_arg_sArr_buffer_ani_item), cpp_arg_sArr_buffer_cpp_item_buf, cpp_arg_sArr_buffer_cpp_item_len + 1, &cpp_arg_sArr_buffer_cpp_item_len);
        cpp_arg_sArr_buffer_cpp_item_buf[cpp_arg_sArr_buffer_cpp_item_len] = '\0';
        cpp_arg_sArr_buffer_cpp_item_tstr.length = cpp_arg_sArr_buffer_cpp_item_len;
        ::taihe::string cpp_arg_sArr_buffer_cpp_item = ::taihe::string(cpp_arg_sArr_buffer_cpp_item_tstr);
        new (&cpp_arg_sArr_buffer[cpp_arg_sArr_buffer_i]) ::taihe::string(std::move(cpp_arg_sArr_buffer_cpp_item));
    }
    ::taihe::array<::taihe::string> cpp_arg_sArr(cpp_arg_sArr_buffer, cpp_arg_sArr_size);
    bool cpp_arg_b = static_cast<bool>(ani_arg_b);
    void* cpp_arg_buffer_data = {};
    ani_size cpp_arg_buffer_length = {};
    env->ArrayBuffer_GetInfo(ani_arg_buffer, &cpp_arg_buffer_data, &cpp_arg_buffer_length);
    ::taihe::array_view<uint8_t> cpp_arg_buffer(reinterpret_cast<uint8_t*>(cpp_arg_buffer_data), cpp_arg_buffer_length);
    ::taihe::string cpp_result = ::string_op::concatString2(cpp_arg_s, cpp_arg_n, cpp_arg_sArr, cpp_arg_b, cpp_arg_buffer);
    if (::taihe::has_error()) { return ani_string{}; }
    ani_string ani_result = {};
    env->String_NewUTF8(cpp_result.c_str(), cpp_result.size(), &ani_result);
    return ani_result;
}
static ani_status ANIFuncsRegister(ani_env *env) {
    ani_module scope;
    if (ANI_OK != env->FindModule("string_op", &scope)) {
        return ANI_ERROR;
    }
    ani_native_function methods[] = {
        {"_taihe_concatString_native", nullptr, reinterpret_cast<void*>(local::concatString)},
        {"_taihe_makeString_native", nullptr, reinterpret_cast<void*>(local::makeString)},
        {"_taihe_split_native", nullptr, reinterpret_cast<void*>(local::split)},
        {"_taihe_split2_native", nullptr, reinterpret_cast<void*>(local::split2)},
        {"_taihe_to_i32_native", nullptr, reinterpret_cast<void*>(local::to_i32)},
        {"_taihe_from_i32_native", nullptr, reinterpret_cast<void*>(local::from_i32)},
        {"_taihe_makePlayStringIface_native", nullptr, reinterpret_cast<void*>(local::makePlayStringIface)},
        {"_taihe_to_f32_native", nullptr, reinterpret_cast<void*>(local::to_f32)},
        {"_taihe_from_f32_native", nullptr, reinterpret_cast<void*>(local::from_f32)},
        {"_taihe_concatString2_native", nullptr, reinterpret_cast<void*>(local::concatString2)},
    };
    return env->Module_BindNativeFunctions(scope, methods, sizeof(methods) / sizeof(ani_native_function));
}
namespace PlayString {
static ani_string pickString([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object, ani_array ani_arg_nums, ani_int ani_arg_n1, ani_int ani_arg_n2) {
    ani_long ani_data_ptr = {};
    env->Object_GetField_Long(object, TH_ANI_FIND_CLASS_FIELD(env, "string_op._taihe_PlayString_inner", "_taihe_dataPtr"), &ani_data_ptr);
    ani_long ani_vtbl_ptr = {};
    env->Object_GetField_Long(object, TH_ANI_FIND_CLASS_FIELD(env, "string_op._taihe_PlayString_inner", "_taihe_vtblPtr"), &ani_vtbl_ptr);
    DataBlockHead* cpp_data_ptr = reinterpret_cast<DataBlockHead*>(ani_data_ptr);
    string_op_PlayString_vtable0* cpp_vtbl_ptr = reinterpret_cast<string_op_PlayString_vtable0*>(ani_vtbl_ptr);
    ::string_op::weak::PlayString cpp_iface = ::string_op::weak::PlayString({cpp_vtbl_ptr, cpp_data_ptr});
    ani_size cpp_arg_nums_size = {};
    env->Array_GetLength(ani_arg_nums, &cpp_arg_nums_size);
    ::taihe::string* cpp_arg_nums_buffer = reinterpret_cast<::taihe::string*>(malloc(cpp_arg_nums_size * sizeof(::taihe::string)));
    for (size_t cpp_arg_nums_buffer_i = 0; cpp_arg_nums_buffer_i < cpp_arg_nums_size; cpp_arg_nums_buffer_i++) {
        ani_ref cpp_arg_nums_buffer_ani_item = {};
        env->Array_Get(ani_arg_nums, cpp_arg_nums_buffer_i, &cpp_arg_nums_buffer_ani_item);
        ani_size cpp_arg_nums_buffer_cpp_item_len = {};
        env->String_GetUTF8Size(static_cast<ani_string>(cpp_arg_nums_buffer_ani_item), &cpp_arg_nums_buffer_cpp_item_len);
        TString cpp_arg_nums_buffer_cpp_item_tstr;
        char* cpp_arg_nums_buffer_cpp_item_buf = tstr_initialize(&cpp_arg_nums_buffer_cpp_item_tstr, cpp_arg_nums_buffer_cpp_item_len + 1);
        env->String_GetUTF8(static_cast<ani_string>(cpp_arg_nums_buffer_ani_item), cpp_arg_nums_buffer_cpp_item_buf, cpp_arg_nums_buffer_cpp_item_len + 1, &cpp_arg_nums_buffer_cpp_item_len);
        cpp_arg_nums_buffer_cpp_item_buf[cpp_arg_nums_buffer_cpp_item_len] = '\0';
        cpp_arg_nums_buffer_cpp_item_tstr.length = cpp_arg_nums_buffer_cpp_item_len;
        ::taihe::string cpp_arg_nums_buffer_cpp_item = ::taihe::string(cpp_arg_nums_buffer_cpp_item_tstr);
        new (&cpp_arg_nums_buffer[cpp_arg_nums_buffer_i]) ::taihe::string(std::move(cpp_arg_nums_buffer_cpp_item));
    }
    ::taihe::array<::taihe::string> cpp_arg_nums(cpp_arg_nums_buffer, cpp_arg_nums_size);
    int32_t cpp_arg_n1 = static_cast<int32_t>(ani_arg_n1);
    int32_t cpp_arg_n2 = static_cast<int32_t>(ani_arg_n2);
    ::taihe::string cpp_result = ::string_op::weak::PlayString(cpp_iface)->pickString(cpp_arg_nums, cpp_arg_n1, cpp_arg_n2);
    if (::taihe::has_error()) { return ani_string{}; }
    ani_string ani_result = {};
    env->String_NewUTF8(cpp_result.c_str(), cpp_result.size(), &ani_result);
    return ani_result;
}
static ani_string getName([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object) {
    ani_long ani_data_ptr = {};
    env->Object_GetField_Long(object, TH_ANI_FIND_CLASS_FIELD(env, "string_op._taihe_PlayString_inner", "_taihe_dataPtr"), &ani_data_ptr);
    ani_long ani_vtbl_ptr = {};
    env->Object_GetField_Long(object, TH_ANI_FIND_CLASS_FIELD(env, "string_op._taihe_PlayString_inner", "_taihe_vtblPtr"), &ani_vtbl_ptr);
    DataBlockHead* cpp_data_ptr = reinterpret_cast<DataBlockHead*>(ani_data_ptr);
    string_op_PlayString_vtable0* cpp_vtbl_ptr = reinterpret_cast<string_op_PlayString_vtable0*>(ani_vtbl_ptr);
    ::string_op::weak::PlayString cpp_iface = ::string_op::weak::PlayString({cpp_vtbl_ptr, cpp_data_ptr});
    ::taihe::string cpp_result = ::string_op::weak::PlayString(cpp_iface)->getName();
    if (::taihe::has_error()) { return ani_string{}; }
    ani_string ani_result = {};
    env->String_NewUTF8(cpp_result.c_str(), cpp_result.size(), &ani_result);
    return ani_result;
}
static void setName([[maybe_unused]] ani_env *env, [[maybe_unused]] ani_object object, ani_string ani_arg_name) {
    ani_long ani_data_ptr = {};
    env->Object_GetField_Long(object, TH_ANI_FIND_CLASS_FIELD(env, "string_op._taihe_PlayString_inner", "_taihe_dataPtr"), &ani_data_ptr);
    ani_long ani_vtbl_ptr = {};
    env->Object_GetField_Long(object, TH_ANI_FIND_CLASS_FIELD(env, "string_op._taihe_PlayString_inner", "_taihe_vtblPtr"), &ani_vtbl_ptr);
    DataBlockHead* cpp_data_ptr = reinterpret_cast<DataBlockHead*>(ani_data_ptr);
    string_op_PlayString_vtable0* cpp_vtbl_ptr = reinterpret_cast<string_op_PlayString_vtable0*>(ani_vtbl_ptr);
    ::string_op::weak::PlayString cpp_iface = ::string_op::weak::PlayString({cpp_vtbl_ptr, cpp_data_ptr});
    ani_size cpp_arg_name_len = {};
    env->String_GetUTF8Size(ani_arg_name, &cpp_arg_name_len);
    TString cpp_arg_name_tstr;
    char* cpp_arg_name_buf = tstr_initialize(&cpp_arg_name_tstr, cpp_arg_name_len + 1);
    env->String_GetUTF8(ani_arg_name, cpp_arg_name_buf, cpp_arg_name_len + 1, &cpp_arg_name_len);
    cpp_arg_name_buf[cpp_arg_name_len] = '\0';
    cpp_arg_name_tstr.length = cpp_arg_name_len;
    ::taihe::string cpp_arg_name = ::taihe::string(cpp_arg_name_tstr);
    ::string_op::weak::PlayString(cpp_iface)->setName(cpp_arg_name);
}
static ani_status ANIMethodsRegister(ani_env *env) {
    ani_class scope;
    if (ANI_OK != env->FindClass("string_op._taihe_PlayString_inner", &scope)) {
        return ANI_ERROR;
    }
    ani_native_function methods[] = {
        {"_taihe_pickString_native", nullptr, reinterpret_cast<void*>(local::PlayString::pickString)},
        {"_taihe_getName_native", nullptr, reinterpret_cast<void*>(local::PlayString::getName)},
        {"_taihe_setName_native", nullptr, reinterpret_cast<void*>(local::PlayString::setName)},
    };
    return env->Class_BindNativeMethods(scope, methods, sizeof(methods) / sizeof(ani_native_function));
}
}
}
namespace string_op {
ani_status ANIRegister(ani_env *env) {
    if (::taihe::get_vm() == nullptr) {
        ani_vm *vm;
        if (ANI_OK != env->GetVM(&vm)) {
            return ANI_ERROR;
        }
        ::taihe::set_vm(vm);
    }
    ani_status status = ANI_OK;
    if (ani_status ret = local::ANIUtilsRegister(env); ret != ANI_OK && ret != ANI_ALREADY_BINDED) {
        std::cerr << "Error from local::ANIUtilsRegister, code: " << ret << std::endl;
        status = ANI_ERROR;
    }
    if (ani_status ret = local::ANIFuncsRegister(env); ret != ANI_OK && ret != ANI_ALREADY_BINDED) {
        std::cerr << "Error from local::ANIFuncsRegister, code: " << ret << std::endl;
        status = ANI_ERROR;
    }
    if (ani_status ret = local::PlayString::ANIMethodsRegister(env); ret != ANI_OK && ret != ANI_ALREADY_BINDED) {
        std::cerr << "Error from local::PlayString::ANIMethodsRegister, code: " << ret << std::endl;
        status = ANI_ERROR;
    }
    return status;
}
}
#pragma clang diagnostic pop
