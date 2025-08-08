#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe/object.hpp"
#include "string_io.ani.hpp"
#include "string_io.user.hpp"
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
    if (ANI_OK != env->FindModule("string_io", &scope)) {
        return ANI_ERROR;
    }
    ani_native_function methods[] = {
        {"_taihe_objDrop", nullptr, reinterpret_cast<void*>(local::_obj_drop)},
        {"_taihe_objDup", nullptr, reinterpret_cast<void*>(local::_obj_dup)},
        {"_taihe_nativeInvoke", nullptr, reinterpret_cast<void*>(local::_native_invoke)},
    };
    return env->Module_BindNativeFunctions(scope, methods, sizeof(methods) / sizeof(ani_native_function));
}
static ani_string input([[maybe_unused]] ani_env *env) {
    ::taihe::string cpp_result = ::string_io::input();
    if (::taihe::has_error()) { return ani_string{}; }
    ani_string ani_result = {};
    env->String_NewUTF8(cpp_result.c_str(), cpp_result.size(), &ani_result);
    return ani_result;
}
static void print([[maybe_unused]] ani_env *env, ani_string ani_arg_a) {
    ani_size cpp_arg_a_len = {};
    env->String_GetUTF8Size(ani_arg_a, &cpp_arg_a_len);
    TString cpp_arg_a_tstr;
    char* cpp_arg_a_buf = tstr_initialize(&cpp_arg_a_tstr, cpp_arg_a_len + 1);
    env->String_GetUTF8(ani_arg_a, cpp_arg_a_buf, cpp_arg_a_len + 1, &cpp_arg_a_len);
    cpp_arg_a_buf[cpp_arg_a_len] = '\0';
    cpp_arg_a_tstr.length = cpp_arg_a_len;
    ::taihe::string cpp_arg_a = ::taihe::string(cpp_arg_a_tstr);
    ::string_io::print(cpp_arg_a);
}
static void println([[maybe_unused]] ani_env *env, ani_string ani_arg_a) {
    ani_size cpp_arg_a_len = {};
    env->String_GetUTF8Size(ani_arg_a, &cpp_arg_a_len);
    TString cpp_arg_a_tstr;
    char* cpp_arg_a_buf = tstr_initialize(&cpp_arg_a_tstr, cpp_arg_a_len + 1);
    env->String_GetUTF8(ani_arg_a, cpp_arg_a_buf, cpp_arg_a_len + 1, &cpp_arg_a_len);
    cpp_arg_a_buf[cpp_arg_a_len] = '\0';
    cpp_arg_a_tstr.length = cpp_arg_a_len;
    ::taihe::string cpp_arg_a = ::taihe::string(cpp_arg_a_tstr);
    ::string_io::println(cpp_arg_a);
}
static ani_status ANIFuncsRegister(ani_env *env) {
    ani_module scope;
    if (ANI_OK != env->FindModule("string_io", &scope)) {
        return ANI_ERROR;
    }
    ani_native_function methods[] = {
        {"_taihe_input_native", nullptr, reinterpret_cast<void*>(local::input)},
        {"_taihe_print_native", nullptr, reinterpret_cast<void*>(local::print)},
        {"_taihe_println_native", nullptr, reinterpret_cast<void*>(local::println)},
    };
    return env->Module_BindNativeFunctions(scope, methods, sizeof(methods) / sizeof(ani_native_function));
}
}
namespace string_io {
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
    return status;
}
}
#pragma clang diagnostic pop
