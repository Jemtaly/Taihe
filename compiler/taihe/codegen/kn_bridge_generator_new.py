from abc import ABCMeta

from typing_extensions import override

from taihe.codegen.abi_generator import IfaceABIInfo
from taihe.semantics.declarations import (
    GlobFuncDecl,
    IfaceDecl,
    IfaceMethodDecl,
    PackageDecl,
    PackageGroup,
    StructDecl,
)
from taihe.semantics.types import (
    BOOL,
    F32,
    F64,
    I8,
    I16,
    I32,
    I64,
    STRING,
    U8,
    U16,
    U32,
    U64,
    # ArrayType,
    BoxType,
    # CallbackType,
    # EnumType,
    IfaceType,
    # MapType,
    ScalarType,
    # SetType,
    SpecialType,
    StructType,
    Type,
    # VectorType,
)
from taihe.semantics.visitor import TypeVisitor
from taihe.utils.analyses import AbstractAnalysis, AnalysisManager
from taihe.utils.outputs import COutputBuffer, OutputManager


class KNBridgePackageInfo(AbstractAnalysis[PackageDecl]):
    def __init__(self, am: AnalysisManager, p: PackageDecl) -> None:
        # self.header = f"{p.name}.api.h"
        self.source = f"{p.name}.api.cpp"
        self.include_impl_header = f"{p.name}.impl.hpp"
        self.include_proj_header = f"{p.name}.proj.hpp"


class GlobFuncDeclKnBridgeInfo(AbstractAnalysis[GlobFuncDecl]):
    def __init__(self, am: AnalysisManager, f: GlobFuncDecl) -> None:
        self.name = f.name


class IfaceMethodDeclKnBridgeInfo(AbstractAnalysis[IfaceMethodDecl]):
    def __init__(self, am: AnalysisManager, f: IfaceMethodDecl) -> None:
        # TODO: Supports projection to any C++ function name based on attributes
        self.name = f.name


class AbstractTypeKnBridgeInfo(metaclass=ABCMeta):
    decl_headers: list[str]
    defn_headers: list[str]
    as_owner: str
    as_field: str
    as_param: str
    as_konan_param: str
    as_konan_field: str
    need_holder: bool
    param_covert_func: str | None
    retval_convert_func_left: str | None
    retval_convert_func_right: str | None
    type_func: str | None

    # def return_from_abi(self, val):
    #     return f"::taihe::core::from_abi<{self.as_field}>({val})"

    # def return_into_abi(self, val):
    #     return f"::taihe::core::into_abi<{self.as_field}>({val})"

    # def pass_from_abi(self, val):
    #     return f"::taihe::core::from_abi<{self.as_param}>({val})"

    # def pass_into_abi(self, val):
    #     return f"::taihe::core::into_abi<{self.as_param}>({val})"


class StructKnBridgeInfo(AbstractAnalysis[StructDecl]):
    def __init__(self, am: AnalysisManager, d: StructDecl) -> None:
        p = d.node_parent
        assert p
        self.pkgname = "::".join(p.segments)
        self.name = d.name
        self.as_owner = self.name
        self.as_param = f"::{self.pkgname}::{self.name}"
        self.as_field = f"::{self.pkgname}::{self.name}"
        self.as_konan_param = "KObjHeader*"
        self.as_konan_field = "KObjHeader*"
        self.type_func = None


class StructTypeKnBridgeInfo(AbstractAnalysis[StructType], AbstractTypeKnBridgeInfo):
    def __init__(self, am: AnalysisManager, t: StructType):
        iface_knbridge_info = StructKnBridgeInfo.get(am, t.ty_decl)
        #  need to do
        self.decl_headers = []
        self.defn_headers = []
        self.pkgname = iface_knbridge_info.pkgname
        self.as_owner = iface_knbridge_info.as_owner
        self.as_field = iface_knbridge_info.as_field
        self.as_param = iface_knbridge_info.as_param
        self.as_konan_param = iface_knbridge_info.as_konan_param
        self.as_konan_field = iface_knbridge_info.as_konan_field
        self.param_covert_func = "THCont_toKotlin"
        self.retval_convert_func_left = (
            f"{self.as_field}{{(uint64_t)CreateStablePointer("
        )
        self.retval_convert_func_right = f")}}"
        self.need_holder = True
        self.type_func = iface_knbridge_info.type_func


class IfaceDeclKnBridgeInfo(AbstractAnalysis[IfaceDecl]):
    def __init__(self, am: AnalysisManager, d: IfaceDecl) -> None:
        p = d.node_parent
        assert p
        self.pkgname = "::".join(p.segments)
        self.name = d.name
        self.as_owner = self.name
        self.as_param = f"::{self.pkgname}::{self.name}"
        self.as_field = f"::{self.pkgname}::{self.name}"
        self.as_konan_param = "KObjHeader*"
        self.as_konan_field = "KObjHeader*"
        if "type_function" not in d.attrs:
            raise KeyError("'type_function' key not found. Exiting program.")
        temp = d.attrs["type_function"].value
        assert isinstance(temp, tuple)
        type_function = temp[0]
        assert isinstance(type_function, str)
        # assert isinstance(temp, str)
        self.type_func: str = type_function


class IfaceTypeKnBridgeInfo(AbstractAnalysis[IfaceType], AbstractTypeKnBridgeInfo):
    def __init__(self, am: AnalysisManager, t: IfaceType):
        iface_knbridge_info = IfaceDeclKnBridgeInfo.get(am, t.ty_decl)
        #  need to do
        self.decl_headers = []
        self.defn_headers = []
        self.pkgname = iface_knbridge_info.pkgname
        self.as_owner = iface_knbridge_info.as_owner
        self.as_field = iface_knbridge_info.as_field
        self.as_param = iface_knbridge_info.as_param
        self.as_konan_param = iface_knbridge_info.as_konan_param
        self.as_konan_field = iface_knbridge_info.as_konan_field
        self.param_covert_func = "THOBJ_toKotlin"
        self.retval_convert_func_left = f"taihe::core::make_holder<{t.ty_decl.name}_impl, {self.pkgname}::{t.ty_decl.name}>(CreateStablePointer("
        self.retval_convert_func_right = f"))"
        self.need_holder = True
        self.type_func = iface_knbridge_info.type_func


class ScalarTypeKnBridgeInfo(AbstractAnalysis[ScalarType], AbstractTypeKnBridgeInfo):
    def __init__(self, am: AnalysisManager, t: ScalarType):
        res = {
            BOOL: "bool",
            F32: "float",
            F64: "double",
            I8: "int8_t",
            I16: "int16_t",
            I32: "int32_t",
            I64: "int64_t",
            U8: "uint8_t",
            U16: "uint16_t",
            U32: "uint32_t",
            U64: "uint64_t",
        }.get(t)
        if res is None:
            raise ValueError
        self.decl_headers = []
        self.defn_headers = []
        self.as_owner = res
        self.as_param = res
        self.as_field = res
        self.as_konan_param = res
        self.as_konan_field = res
        self.param_covert_func = None
        self.retval_convert_func_left = None
        self.retval_convert_func_right = None
        self.need_holder = False
        self.type_func = None


class SpecialTypeKnBridgeInfo(AbstractAnalysis[SpecialType], AbstractTypeKnBridgeInfo):
    def __init__(self, am: AnalysisManager, t: SpecialType):
        if t != STRING:
            raise ValueError
        self.decl_headers = ["core/string.hpp"]
        self.defn_headers = ["core/string.hpp"]
        self.as_owner = "string"
        self.as_field = "::taihe::core::string"
        self.as_param = "::taihe::core::string_view"
        self.as_konan_param = "KObjHeader*"
        self.as_konan_field = "KObjHeader*"
        self.param_covert_func = "CreateStringFromTHString"
        self.retval_convert_func_left = "CreateTHStringFromString("
        self.retval_convert_func_right = ")"
        self.need_holder = True
        self.type_func = None


class BoxTypeKnBridgeInfo(AbstractAnalysis[BoxType], AbstractTypeKnBridgeInfo):
    def __init__(self, am: AnalysisManager, t: BoxType):
        self.decl_headers = ["taihe/box.abi.h"]
        self.defn_headers = ["taihe/box.abi.h"]
        self.as_owner = "struct TBox"
        self.as_field = "struct TBox"
        self.as_param = "struct TBox"
        self.as_konan_param = "KObjHeader*"
        self.as_konan_field = "KObjHeader*"
        self.param_covert_func = "thbox_tokotlin"
        self.retval_convert_func_left = "struct TBox{CreateStablePointer("
        self.retval_convert_func_right = ")}"
        self.need_holder = True
        self.type_func = None


class TypeKnBridgeInfo(TypeVisitor[AbstractTypeKnBridgeInfo]):
    def __init__(self, am: AnalysisManager):
        self.am = am

    @staticmethod
    def get(am: AnalysisManager, t: Type | None) -> AbstractTypeKnBridgeInfo:
        assert t is not None
        return TypeKnBridgeInfo(am).handle_type(t)

    @override
    def visit_iface_type(self, t: IfaceType) -> AbstractTypeKnBridgeInfo:
        return IfaceTypeKnBridgeInfo.get(self.am, t)

    @override
    def visit_scalar_type(self, t: ScalarType) -> AbstractTypeKnBridgeInfo:
        return ScalarTypeKnBridgeInfo.get(self.am, t)

    @override
    def visit_special_type(self, t: SpecialType) -> AbstractTypeKnBridgeInfo:
        return SpecialTypeKnBridgeInfo.get(self.am, t)

    @override
    def visit_struct_type(self, t: StructType) -> AbstractTypeKnBridgeInfo:
        return StructTypeKnBridgeInfo.get(self.am, t)

    # @override
    # def visit_array_type(self, t: ArrayType) -> AbstractTypeKnBridgeInfo:
    #     return ArrayTypeKnBridgeInfo.get(self.am, t)


class KNBridgeCodeGenerator:
    def __init__(self, tm: OutputManager, am: AnalysisManager):
        self.tm = tm
        self.am = am
        self.kn_predefined_type_list = [
            "Byte",
            "Short",
            "Int",
            "Long",
            "Float",
            "Double",
            "Char",
            "Boolean",
            "Unit",
            "UByte",
            "UShort",
            "UInt",
            "ULong",
        ]
        self.dict_konan_func = {}
        self.dict_params = {}
        self.dict_params_only_var = {}

    def generate(self, pg: PackageGroup):
        for pkg in pg.packages:
            # self.gen_package_header_file(pkg)
            self.gen_package_source_file(pkg)

    def gen_package_source_file(self, pkg: PackageDecl):
        kn_bridge_pkg_info = KNBridgePackageInfo.get(self.am, pkg)
        kn_bridge_pkg_target = COutputBuffer.create(
            self.tm, f"src/{kn_bridge_pkg_info.source}", False
        )

        if "prefix" not in pkg.attrs:
            raise KeyError("'prefix' key not found. Exiting program.")
        temp = pkg.attrs["prefix"].value
        assert isinstance(temp, tuple)
        kn_bridge_prefix_ = temp[0]
        assert isinstance(kn_bridge_prefix_, str)
        # assert isinstance(temp, str)
        kn_bridge_prefix: str = kn_bridge_prefix_

        kn_bridge_pkg_target.include("core/string.hpp")
        kn_bridge_pkg_target.include(kn_bridge_pkg_info.include_impl_header)
        kn_bridge_pkg_target.include(kn_bridge_pkg_info.include_proj_header)
        kn_bridge_pkg_target.write(f"#include <stdint.h>\n")

        self.gen_need_decl(pkg, kn_bridge_pkg_target, kn_bridge_prefix)
        self.def_macro(pkg, kn_bridge_pkg_target)
        self.gen_package_th_tydef(pkg, kn_bridge_pkg_target)
        self.gen_funcdecl(pkg, kn_bridge_pkg_target)
        self.gen_class_impl(pkg, kn_bridge_pkg_target, kn_bridge_prefix)
        self.gen_func_impl(pkg, kn_bridge_pkg_target, kn_bridge_prefix)
        self.gen_func_macro(pkg, kn_bridge_pkg_target)
        self.undef_macro(pkg, kn_bridge_pkg_target)

    def gen_need_decl(
        self,
        pkg: PackageDecl,
        kn_bridge_pkg_target: COutputBuffer,
        kn_bridge_prefix: str,
    ):
        kn_bridge_pkg_target.write(
            f"struct {kn_bridge_prefix}_KType;\n"
            f"typedef struct {kn_bridge_prefix}_KType {kn_bridge_prefix}_KType;\n"
            f"struct KObjHeader;\n"
            f"typedef struct KObjHeader KObjHeader;\n"
            f"struct KTypeInfo;\n"
            f"typedef struct KTypeInfo KTypeInfo;\n"
            f"struct FrameOverlay;\n"
            f"typedef struct FrameOverlay FrameOverlay;\n"
            f"\n"
            f"#define RUNTIME_NOTHROW __attribute__((nothrow))\n"
            f"\n"
            f"#if __has_attribute(retain)\n"
            f"#define RUNTIME_EXPORT __attribute__((used,retain))\n"
            f"#else\n"
            f"#define RUNTIME_EXPORT __attribute__((used))\n"
            f"#endif\n"
            f"\n"
            f"#define RUNTIME_NORETURN __attribute__((noreturn))\n"
            f"\n"
            f'extern "C" {{\n'
            f"  void UpdateStackRef(KObjHeader**, const KObjHeader*) RUNTIME_NOTHROW;\n"
            f"  KObjHeader* AllocInstance(const KTypeInfo*, KObjHeader**) RUNTIME_NOTHROW;\n"
            f"  KObjHeader* DerefStablePointer(void*, KObjHeader**) RUNTIME_NOTHROW;\n"
            f"  void* CreateStablePointer(KObjHeader*) RUNTIME_NOTHROW;\n"
            f"  void DisposeStablePointer(void*) RUNTIME_NOTHROW;\n"
            f"  bool IsInstanceInternal(const KObjHeader*, const KTypeInfo*) RUNTIME_NOTHROW;\n"
            f"  void EnterFrame(KObjHeader** start, int parameters, int count) RUNTIME_NOTHROW;\n"
            f"  void LeaveFrame(KObjHeader** start, int parameters, int count) RUNTIME_NOTHROW;\n"
            f"  void SetCurrentFrame(KObjHeader** start) RUNTIME_NOTHROW;\n"
            f"  FrameOverlay* getCurrentFrame() RUNTIME_NOTHROW;\n"
            f"  void Kotlin_initRuntimeIfNeeded();\n"
            f"  void Kotlin_mm_switchThreadStateRunnable() RUNTIME_NOTHROW;\n"
            f"  void Kotlin_mm_switchThreadStateNative() RUNTIME_NOTHROW;\n"
            f"  void HandleCurrentExceptionWhenLeavingKotlinCode();\n"
            f"\n"
            f"  KObjHeader* CreateStringFromCString(const char*, KObjHeader**);\n"
            f"  char* CreateCStringFromString(const KObjHeader*);\n"
            f"  KObjHeader* CreateStringFromTHString(taihe::core::string_view, KObjHeader**);\n"
            f"  taihe::core::string CreateTHStringFromString(const KObjHeader*);\n"
            f"  void DisposeCString(char* cstring);\n"
            f"}}\n"
            f"\n"
            f"struct lib_FrameOverlay {{\n"
            f"  lib_FrameOverlay* previous;\n"
            f"  int parameters;\n"
            f"  int count;\n"
            f"}};\n"
            f"\n"
            f"class KObjHolder {{\n"
            f"public:\n"
            f"  KObjHolder() : obj_(nullptr) {{\n"
            f"    EnterFrame(frame(), 0, sizeof(*this)/sizeof(void*));\n"
            f"  }}\n"
            f"  explicit KObjHolder(const KObjHeader* obj) : obj_(nullptr) {{\n"
            f"    EnterFrame(frame(), 0, sizeof(*this)/sizeof(void*));\n"
            f"    UpdateStackRef(&obj_, obj);\n"
            f"  }}\n"
            f"  ~KObjHolder() {{\n"
            f"    LeaveFrame(frame(), 0, sizeof(*this)/sizeof(void*));\n"
            f"  }}\n"
            f"  KObjHeader* obj() {{ return obj_; }}\n"
            f"  KObjHeader** slot() {{ return &obj_; }}\n"
            f"private:\n"
            f"  lib_FrameOverlay frame_;\n"
            f"  KObjHeader* obj_;\n"
            f"\n"
            f"  KObjHeader** frame() {{ return reinterpret_cast<KObjHeader**>(&frame_); }}\n"
            f"}};\n"
            f"\n"
            f"class ScopedRunnableState {{\n"
            f"public:\n"
            f"  ScopedRunnableState() noexcept {{ Kotlin_mm_switchThreadStateRunnable(); }}\n"
            f"  ~ScopedRunnableState() {{ Kotlin_mm_switchThreadStateNative(); }}\n"
            f"  ScopedRunnableState(const ScopedRunnableState&) = delete;\n"
            f"  ScopedRunnableState(ScopedRunnableState&&) = delete;\n"
            f"  ScopedRunnableState& operator=(const ScopedRunnableState&) = delete;\n"
            f"  ScopedRunnableState& operator=(ScopedRunnableState&&) = delete;\n"
            f"}};\n"
            f"\n"
            f"KObjHeader* thobj_tokotlin(void* thiface, KObjHeader** kobj_slot) {{\n"
            f"  void* data_ptr = *(void**)((char*)thiface + 8);\n"
            f"  void* kn_obj = *(void**)((char*)data_ptr + 16);\n"
            f"  KObjHeader* result = DerefStablePointer(kn_obj, kobj_slot);\n"
            f"  return result;\n"
            f"}}\n"
            f"\n"
            # f"KObjHeader* thbox_tokotlin(struct TBox box, KObjHeader** kobj_slot) {{\n"
            # f"  void* data_ptr = (void*)box;\n"
            # f"  KObjHeader* result = DerefStablePointer(data_ptr, kobj_slot);\n"
            # f"  return result;\n"
            # f"}}\n"
            # f"\n"
        )

    def def_macro(self, pkg: PackageDecl, kn_bridge_pkg_target: COutputBuffer):
        kn_bridge_pkg_target.write(
            f"#define THOBJ_toKotlin(obj, obj_slot) thobj_tokotlin((void*)&obj, obj_slot)\n"
            f"#define THCont_toKotlin(container, container_slot) DerefStablePointer((void*)container.ptr, container_slot)"
            f"\n"
        )

    def gen_package_th_tydef(
        self, pkg: PackageDecl, kn_bridge_pkg_target: COutputBuffer
    ):
        for iface in pkg.interfaces:
            kn_bridge_pkg_target.write(f"typedef void* Kref_{iface.name};\n" f"\n")

    def gen_funcdecl(self, pkg: PackageDecl, kn_bridge_pkg_target: COutputBuffer):
        for iface in pkg.interfaces:
            iface_name = iface.name
            for func in iface.methods:
                self.gen_iface_method_decl(iface_name, func, kn_bridge_pkg_target)
        for func in pkg.functions:
            if "constructor" not in func.attrs and "singleton" not in func.attrs:
                self.gen_toplevel_method_decl(func, kn_bridge_pkg_target)

    def gen_class_impl(
        self,
        pkg: PackageDecl,
        kn_bridge_pkg_target: COutputBuffer,
        kn_bridge_prefix: str,
    ):
        for iface in pkg.interfaces:
            if "object_kind" not in iface.attrs:
                raise KeyError("'object_kind' key not found. Exiting program.")
            if "object_kind" in iface.attrs:
                attrslist = iface.attrs["object_kind"].value
                assert isinstance(attrslist, tuple)
                if (
                    attrslist[0] == "class"
                    or attrslist[0] == "object"
                    or attrslist[0] == "interface"
                ):
                    # if "object_kind" in iface.attrs and (
                    #     iface.attrs["object_kind"].value[0] == "class"
                    #     or iface.attrs["object_kind"][0].value[0] == "object"
                    # ):
                    self.gen_class(iface, kn_bridge_pkg_target)

    def gen_func_impl(
        self,
        pkg: PackageDecl,
        kn_bridge_pkg_target: COutputBuffer,
        kn_bridge_prefix: str,
    ):
        # self.gen_iface_method_impl(pkg, kn_bridge_pkg_target, kn_bridge_prefix)
        for iface in pkg.interfaces:
            self.gen_iface_type_method_decl(
                iface, kn_bridge_pkg_target, kn_bridge_prefix
            )
            # self.gen_iface_init_method(iface, kn_bridge_pkg_target, kn_bridge_prefix)
            iface_name = iface.name
            for func in iface.methods:
                self.gen_iface_method_impl(iface_name, func, kn_bridge_pkg_target)
        for func in pkg.functions:
            if "constructor" not in func.attrs and "singleton" not in func.attrs:
                self.gen_toplevel_func_impl(func, kn_bridge_pkg_target)
            elif "constructor" in func.attrs:
                str1, str2, str3 = self.gen_class_init_func_impl(
                    func, kn_bridge_pkg_target, True
                )
                func_ret_ref = func.return_ty_ref
                assert func_ret_ref is not None
                assert hasattr(func_ret_ref, "resolved_ty")
                iface_temp = func_ret_ref.resolved_ty
                assert isinstance(iface_temp, IfaceType)
                ##################
                # need to delete #
                ##################
                self.dict_params[iface_temp.ty_decl.name] = str1
                self.dict_params_only_var[iface_temp.ty_decl.name] = str2
                self.dict_konan_func[iface_temp.ty_decl.name] = str3
                ##################
                #  need to add   #
                ##################
                # self.gen_makeclass(func, kn_bridge_pkg_target, str1, str2, str3)
            elif "singleton" in func.attrs:
                str1, str2, str3 = self.gen_obj_init_func_impl(
                    func, kn_bridge_pkg_target, True
                )
                func_ret_ref = func.return_ty_ref
                assert func_ret_ref is not None
                assert hasattr(func_ret_ref, "resolved_ty")
                iface_temp = func_ret_ref.resolved_ty
                assert isinstance(iface_temp, IfaceType)
                ##################
                # need to delete #
                ##################
                self.dict_params[iface_temp.ty_decl.name] = str1
                self.dict_params_only_var[iface_temp.ty_decl.name] = str2
                self.dict_konan_func[iface_temp.ty_decl.name] = str3
                ##################
                #  need to add   #
                ##################
                # self.gen_makeclass(func, kn_bridge_pkg_target, str1, str2, str3)

        for func in pkg.functions:
            if "constructor" in func.attrs:
                str1, str2, str3 = self.gen_class_init_func_impl(
                    func, kn_bridge_pkg_target, False
                )
                self.gen_makeclass(func, kn_bridge_pkg_target, str1, str2, str3)
            elif "singleton" in func.attrs:
                str1, str2, str3 = self.gen_obj_init_func_impl(
                    func, kn_bridge_pkg_target, False
                )
                self.gen_makeclass(func, kn_bridge_pkg_target, str1, str2, str3)

    def gen_obj_init_func_impl(
        self, func: GlobFuncDecl, kn_bridge_pkg_target: COutputBuffer, gen: bool
    ):
        if "inner_name" not in func.attrs:
            raise KeyError("'inner_name' key not found. Exiting program.")
        konan_proj_name_list = func.attrs["inner_name"].value
        assert isinstance(konan_proj_name_list, tuple)
        konan_proj_name = konan_proj_name_list[0]
        assert isinstance(konan_proj_name, str)
        params_holder = []
        params = []
        konan_params_only_ty = []
        convert_params = []
        params_only_var = []
        need_ret_holder = False

        for param in func.params:
            type_knbridge_info = TypeKnBridgeInfo.get(self.am, param.ty_ref.resolved_ty)
            params.append(f"{type_knbridge_info.as_param} {param.name}")
            params_holder.append(type_knbridge_info.need_holder)
            konan_params_only_ty.append(f"{type_knbridge_info.as_konan_param}")
            params_only_var.append(f"{param.name}")

            convert_params.append(
                f"{type_knbridge_info.param_covert_func}({param.name}, {param.name}_holder.slot())"
                if type_knbridge_info.need_holder
                else f"{param.name}"
            )

        return_ty_name, return_ty_str, return_ty_konan_name, need_ret_holder = (
            self._process_return_type(func)
        )

        if need_ret_holder:
            konan_params_only_ty.append("KObjHeader**")
            convert_params.append("result_holder.slot()")

        params_str = ", ".join(params)
        konan_params_only_ty_str = ", ".join(konan_params_only_ty)
        convert_params_str = ", ".join(convert_params)
        params_only_var_str = ", ".join(params_only_var)

        if gen:
            self._generate_objinit_function_body(
                kn_bridge_pkg_target,
                func,
                konan_proj_name,
                return_ty_konan_name,
                return_ty_name,
                params_str,
                konan_params_only_ty_str,
                convert_params_str,
                return_ty_str,
                params_holder,
                need_ret_holder,
                False,
            )
        return params_str, params_only_var_str, konan_proj_name

    def gen_iface_method_decl(
        self,
        iface_name: str,
        func: IfaceMethodDecl,
        kn_bridge_pkg_target: COutputBuffer,
    ):
        if "inner_name" not in func.attrs:
            raise KeyError("'inner_name' key not found. Exiting program.")
        konan_proj_name_list = func.attrs["inner_name"].value
        assert isinstance(konan_proj_name_list, tuple)
        konan_proj_name = konan_proj_name_list[0]
        assert isinstance(konan_proj_name, str)
        params = []

        params.append(f"Kref_{iface_name} thiz")
        for param in func.params:
            type_knbridge_info = TypeKnBridgeInfo.get(self.am, param.ty_ref.resolved_ty)
            params.append(f"{type_knbridge_info.as_param} {param.name}")

        return_ty_name, _, _, _ = self._process_return_type(func)
        params_str = ", ".join(params)
        kn_bridge_pkg_target.write(
            f"static {return_ty_name} {konan_proj_name}_impl({params_str});\n" f"\n"
        )

    def gen_toplevel_method_decl(
        self, func: GlobFuncDecl, kn_bridge_pkg_target: COutputBuffer
    ):
        if "inner_name" not in func.attrs:
            raise KeyError("'inner_name' key not found. Exiting program.")
        konan_proj_name_list = func.attrs["inner_name"].value
        assert isinstance(konan_proj_name_list, tuple)
        konan_proj_name = konan_proj_name_list[0]
        assert isinstance(konan_proj_name, str)
        params = []

        for param in func.params:
            type_knbridge_info = TypeKnBridgeInfo.get(self.am, param.ty_ref.resolved_ty)
            params.append(f"{type_knbridge_info.as_param} {param.name}")

        return_ty_name, _, _, _ = self._process_return_type(func)

        params_str = ",".join(params)

        kn_bridge_pkg_target.write(
            f"static {return_ty_name} {konan_proj_name}_impl({params_str});\n" f"\n"
        )

    def gen_makeclass(
        self,
        func: GlobFuncDecl,
        kn_bridge_pkg_target: COutputBuffer,
        params: str,
        params_only_var: str,
        konan_proj_name: str,
    ):
        return_ty_ref = func.return_ty_ref
        assert return_ty_ref is not None
        assert hasattr(return_ty_ref, "resolved_ty")
        type_knbridge_info = TypeKnBridgeInfo.get(self.am, return_ty_ref.resolved_ty)
        th_iface_type = type_knbridge_info.as_field
        # iface_impl_type = type_knbridge_info.as_owner
        kn_bridge_pkg_target.write(
            f"{th_iface_type} {func.name}_author({params}) {{\n"
            # f"  return make_holder<{iface_impl_type}_impl, {th_iface_type}>
            # ({konan_proj_name}_impl({params_only_var}));\n"
            f"  return {konan_proj_name}_impl({params_only_var});\n"
            f"}}\n"
            f"\n"
        )

    def gen_iface_type_method_decl(
        self,
        iface: IfaceDecl,
        kn_bridge_pkg_target: COutputBuffer,
        kn_bridge_prefix: str,
    ):
        iface_info = IfaceDeclKnBridgeInfo.get(self.am, iface)
        kn_bridge_pkg_target.write(
            f'extern "C" {kn_bridge_prefix}_KType* {iface_info.type_func}(void);\n'
        )

    def gen_iface_method_impl(
        self,
        iface_name: str,
        func: IfaceMethodDecl,
        kn_bridge_pkg_target: COutputBuffer,
    ):
        if "inner_name" not in func.attrs:
            raise KeyError("'inner_name' key not found. Exiting program.")
        konan_proj_name_list = func.attrs["inner_name"].value
        assert isinstance(konan_proj_name_list, tuple)
        konan_proj_name = konan_proj_name_list[0]
        assert isinstance(konan_proj_name, str)
        params_holder = []
        params = []
        konan_params_only_ty = []
        convert_params = []
        need_ret_holder = False

        params.append(f"Kref_{iface_name} thiz")
        konan_params_only_ty.append(f"KObjHeader*")
        convert_params.append(f"DerefStablePointer(thiz, thiz_holder.slot())")

        for param in func.params:
            type_knbridge_info = TypeKnBridgeInfo.get(self.am, param.ty_ref.resolved_ty)
            params.append(f"{type_knbridge_info.as_param} {param.name}")
            params_holder.append(type_knbridge_info.need_holder)
            konan_params_only_ty.append(f"{type_knbridge_info.as_konan_param}")

            convert_params.append(
                f"{type_knbridge_info.param_covert_func}({param.name}, {param.name}_holder.slot())"
                if type_knbridge_info.need_holder
                else f"{param.name}"
            )

        return_ty_name, return_ty_str, return_ty_konan_name, need_ret_holder = (
            self._process_return_type(func)
        )

        if need_ret_holder:
            konan_params_only_ty.append("KObjHeader**")
            convert_params.append("result_holder.slot()")

        params_str = ", ".join(params)
        konan_params_only_ty_str = ", ".join(konan_params_only_ty)
        convert_params_str = ", ".join(convert_params)

        self._generate_function_body(
            kn_bridge_pkg_target,
            func,
            konan_proj_name,
            return_ty_konan_name,
            return_ty_name,
            params_str,
            konan_params_only_ty_str,
            convert_params_str,
            return_ty_str,
            params_holder,
            need_ret_holder,
            True,
        )

    def gen_toplevel_func_impl(
        self, func: GlobFuncDecl, kn_bridge_pkg_target: COutputBuffer
    ):
        if "inner_name" not in func.attrs:
            raise KeyError("'inner_name' key not found. Exiting program.")
        konan_proj_name_list = func.attrs["inner_name"].value
        assert isinstance(konan_proj_name_list, tuple)
        konan_proj_name = konan_proj_name_list[0]
        assert isinstance(konan_proj_name, str)
        params_holder = []
        params = []
        konan_params_only_ty = []
        convert_params = []
        need_ret_holder = False

        for param in func.params:
            type_knbridge_info = TypeKnBridgeInfo.get(self.am, param.ty_ref.resolved_ty)
            params.append(f"{type_knbridge_info.as_param} {param.name}")
            params_holder.append(type_knbridge_info.need_holder)
            konan_params_only_ty.append(f"{type_knbridge_info.as_konan_param}")

            convert_params.append(
                f"{type_knbridge_info.param_covert_func}({param.name}, {param.name}_holder.slot())"
                if type_knbridge_info.need_holder
                else f"{param.name}"
            )

        return_ty_name, return_ty_str, return_ty_konan_name, need_ret_holder = (
            self._process_return_type(func)
        )

        if need_ret_holder:
            konan_params_only_ty.append("KObjHeader**")
            convert_params.append("result_holder.slot()")

        params_str = ", ".join(params)
        konan_params_only_ty_str = ", ".join(konan_params_only_ty)
        convert_params_str = ", ".join(convert_params)

        self._generate_function_body(
            kn_bridge_pkg_target,
            func,
            konan_proj_name,
            return_ty_konan_name,
            return_ty_name,
            params_str,
            konan_params_only_ty_str,
            convert_params_str,
            return_ty_str,
            params_holder,
            need_ret_holder,
            False,
        )

    def _process_return_type(self, func: GlobFuncDecl | IfaceMethodDecl):
        if func.return_ty_ref is None:
            return "void", "", "void", False

        return_ty_ref = func.return_ty_ref
        assert return_ty_ref is not None
        assert hasattr(return_ty_ref, "resolved_ty")
        type_knbridge_info = TypeKnBridgeInfo.get(self.am, return_ty_ref.resolved_ty)
        need_ret_holder = type_knbridge_info.need_holder
        return_ty_name = type_knbridge_info.as_field
        return_ty_konan_name = type_knbridge_info.as_konan_field

        return_ty_str = (
            f"{type_knbridge_info.retval_convert_func_left}result{type_knbridge_info.retval_convert_func_right}"
            if need_ret_holder
            else "result"
        )
        return return_ty_name, return_ty_str, return_ty_konan_name, need_ret_holder

    def _generate_objinit_function_body(
        self,
        kn_bridge_pkg_target: COutputBuffer,
        func: GlobFuncDecl | IfaceMethodDecl,
        konan_proj_name: str,
        return_ty_konan_name: str,
        return_ty_name: str,
        params_str: str,
        konan_params_only_ty_str: str,
        convert_params_str: str,
        return_ty_str: str,
        params_holder: list,
        need_ret_holder: bool,
        need_thiz_holder: bool,
    ):
        kn_bridge_pkg_target.write(
            f'extern "C" {return_ty_konan_name} {konan_proj_name} ({konan_params_only_ty_str});\n'
        )
        if need_thiz_holder:
            kn_bridge_pkg_target.write(
                f"static {return_ty_name} {konan_proj_name}_impl({params_str}) {{\n"
            )
        else:
            kn_bridge_pkg_target.write(
                f"static {return_ty_name} {konan_proj_name}_impl({params_str}) {{\n"
            )
        kn_bridge_pkg_target.write(
            f"  Kotlin_initRuntimeIfNeeded();\n" f"  ScopedRunnableState stateGuard;\n"
        )

        for index, need_holder in enumerate(params_holder):
            if need_holder:
                kn_bridge_pkg_target.write(
                    f"  KObjHolder {func.params[index].name}_holder;\n"
                )

        if need_thiz_holder:
            kn_bridge_pkg_target.write(f"  KObjHolder thiz_holder;\n")

        # kn_bridge_pkg_target.write(f"  try {{\n")

        if return_ty_name == "void":
            kn_bridge_pkg_target.write(
                f"    {konan_proj_name}({convert_params_str});\n"
            )
        else:
            if need_ret_holder:
                kn_bridge_pkg_target.write(f"  KObjHolder result_holder;\n")
            kn_bridge_pkg_target.write(
                f"  auto result = {konan_proj_name}({convert_params_str});\n"
                f"  return {return_ty_str};\n"
            )

        kn_bridge_pkg_target.write(
            # f"  }} catch (...) {{\n"
            # f"    SetCurrentFrame(reinterpret_cast<KObjHeader**>(frame));\n"
            # f"    HandleCurrentExceptionWhenLeavingKotlinCode();\n"
            # f"  }} \n"
            f"}}\n"
            f"\n"
        )

    def _generate_function_body(
        self,
        kn_bridge_pkg_target: COutputBuffer,
        func: GlobFuncDecl | IfaceMethodDecl,
        konan_proj_name: str,
        return_ty_konan_name: str,
        return_ty_name: str,
        params_str: str,
        konan_params_only_ty_str: str,
        convert_params_str: str,
        return_ty_str: str,
        params_holder: list,
        need_ret_holder: bool,
        need_thiz_holder: bool,
    ):
        kn_bridge_pkg_target.write(
            f'extern "C" {return_ty_konan_name} {konan_proj_name} ({konan_params_only_ty_str});\n'
        )
        if need_thiz_holder:
            kn_bridge_pkg_target.write(
                f"static {return_ty_name} {konan_proj_name}_impl({params_str}) {{\n"
            )
        else:
            kn_bridge_pkg_target.write(
                f"static {return_ty_name} {konan_proj_name}_impl({params_str}) {{\n"
            )
        kn_bridge_pkg_target.write(
            f"  Kotlin_initRuntimeIfNeeded();\n"
            f"  ScopedRunnableState stateGuard;\n"
            f"  FrameOverlay* frame = getCurrentFrame();\n"
        )

        for index, need_holder in enumerate(params_holder):
            if need_holder:
                kn_bridge_pkg_target.write(
                    f"  KObjHolder {func.params[index].name}_holder;\n"
                )

        if need_thiz_holder:
            kn_bridge_pkg_target.write(f"  KObjHolder thiz_holder;\n")

        kn_bridge_pkg_target.write(f"  try {{\n")

        if return_ty_name == "void":
            kn_bridge_pkg_target.write(
                f"    {konan_proj_name}({convert_params_str});\n"
            )
        else:
            if need_ret_holder:
                kn_bridge_pkg_target.write(f"    KObjHolder result_holder;\n")
            kn_bridge_pkg_target.write(
                f"    auto result = {konan_proj_name}({convert_params_str});\n"
                f"    return {return_ty_str};\n"
            )

        kn_bridge_pkg_target.write(
            f"  }} catch (...) {{\n"
            f"    SetCurrentFrame(reinterpret_cast<KObjHeader**>(frame));\n"
            f"    HandleCurrentExceptionWhenLeavingKotlinCode();\n"
            f"  }} \n"
            f"}}\n"
            f"\n"
        )

    def _generate_init_function_body(
        self,
        kn_bridge_pkg_target: COutputBuffer,
        func: GlobFuncDecl | IfaceMethodDecl,
        konan_proj_name: str,
        return_ty_konan_name: str,
        return_ty_name: str,
        params_str: str,
        konan_params_only_ty_str: str,
        convert_params_str: str,
        return_ty_str: str,
        params_holder: list,
        need_ret_holder: bool,
    ):
        return_ty_ref = func.return_ty_ref
        assert return_ty_ref is not None
        assert hasattr(return_ty_ref, "resolved_ty")
        iface_temp = return_ty_ref.resolved_ty
        assert isinstance(iface_temp, IfaceType)
        type_func = IfaceDeclKnBridgeInfo.get(self.am, iface_temp.ty_decl).type_func

        kn_bridge_pkg_target.write(
            f'extern "C" {return_ty_konan_name} {konan_proj_name} ({konan_params_only_ty_str});\n'
        )
        kn_bridge_pkg_target.write(
            f"static {return_ty_name} {konan_proj_name}_impl({params_str}) {{\n"
        )
        kn_bridge_pkg_target.write(
            f"  Kotlin_initRuntimeIfNeeded();\n"
            f"  ScopedRunnableState stateGuard;\n"
            f"  FrameOverlay* frame = getCurrentFrame();\n"
        )

        for index, need_holder in enumerate(params_holder):
            if need_holder:
                kn_bridge_pkg_target.write(
                    f"  KObjHolder {func.params[index].name}_holder;\n"
                )

        kn_bridge_pkg_target.write(f"  try {{\n")

        if need_ret_holder:
            kn_bridge_pkg_target.write(f"    KObjHolder result_holder;\n")

        kn_bridge_pkg_target.write(
            f"    KObjHeader* result = AllocInstance((KTypeInfo*){type_func}(), result_holder.slot());\n"
            f"    {konan_proj_name}(result{', ' + convert_params_str if convert_params_str else ''});\n"
            f"    return {return_ty_str};\n"
        )

        kn_bridge_pkg_target.write(
            f"  }} catch (...) {{\n"
            f"    SetCurrentFrame(reinterpret_cast<KObjHeader**>(frame));\n"
            f"    HandleCurrentExceptionWhenLeavingKotlinCode();\n"
            f"  }} \n"
            f"}}\n"
            f"\n"
        )

    def gen_class_init_func_impl(
        self, func: GlobFuncDecl, kn_bridge_pkg_target: COutputBuffer, gen: bool
    ):
        if "inner_name" not in func.attrs:
            raise KeyError("'inner_name' key not found. Exiting program.")
        konan_proj_name_list = func.attrs["inner_name"].value
        assert isinstance(konan_proj_name_list, tuple)
        konan_proj_name = konan_proj_name_list[0]
        assert isinstance(konan_proj_name, str)
        params_holder = []
        params = []
        params_only_var = []
        konan_params_only_ty = []
        convert_params = []
        need_ret_holder = True

        konan_params_only_ty.append("KObjHeader*")
        for param in func.params:
            type_knbridge_info = TypeKnBridgeInfo.get(self.am, param.ty_ref.resolved_ty)
            params.append(f"{type_knbridge_info.as_param} {param.name}")
            params_holder.append(type_knbridge_info.need_holder)
            params_only_var.append(f"{param.name}")
            konan_params_only_ty.append(f"{type_knbridge_info.as_konan_param}")

            convert_params.append(
                f"{type_knbridge_info.param_covert_func}({param.name}, {param.name}_holder.slot())"
                if type_knbridge_info.need_holder
                else f"{param.name}"
            )

        return_ty_name, return_ty_str, _, __ = self._process_return_type(func)
        return_ty_konan_name = "void"

        params_str = ", ".join(params)
        params_only_var_str = ", ".join(params_only_var)
        konan_params_only_ty_str = ", ".join(konan_params_only_ty)
        convert_params_str = ", ".join(convert_params)

        if gen:
            self._generate_init_function_body(
                kn_bridge_pkg_target,
                func,
                konan_proj_name,
                return_ty_konan_name,
                return_ty_name,
                params_str,
                konan_params_only_ty_str,
                convert_params_str,
                return_ty_str,
                params_holder,
                need_ret_holder,
            )
        return params_str, params_only_var_str, konan_proj_name

    def gen_class(self, iface: IfaceDecl, kn_bridge_pkg_target: COutputBuffer):
        # pkgname = IfaceDeclKnBridgeInfo.get(self.am, iface).pkgname
        kn_bridge_pkg_target.write(
            f"struct {iface.name}_impl {{\n"
            f"protected:\n"
            f"  void* m_handle;\n"
            f"public:\n"
        )

        self._gen_ctor(iface, kn_bridge_pkg_target)

        kn_bridge_pkg_target.write(
            f"  ~{iface.name}_impl() {{\n"
            f"    DisposeStablePointer(m_handle);\n"
            f"    m_handle = nullptr;\n"
            f"  }}\n"
            f"\n"
        )
        # perantsList = IfaceABIInfo.get(self.am, iface)
        self._gen_class_all_func_bind(iface, kn_bridge_pkg_target)

        # for func in iface.methods:
        #     self._gen_class_func_bind(func, kn_bridge_pkg_target)

        kn_bridge_pkg_target.write(f"}};\n" f"\n")

    def _gen_class_all_func_bind(
        self, iface: IfaceDecl, kn_bridge_pkg_target: COutputBuffer
    ):
        perantsList = IfaceABIInfo.get(self.am, iface).ancestor_dict
        class_func_list = []
        if not perantsList:
            return
        for ifaceperant in perantsList:
            for func in ifaceperant.methods:
                if func.name not in class_func_list:
                    class_func_list.append(func.name)
                    self._gen_class_func_bind(func, kn_bridge_pkg_target)

    def _gen_ctor(self, iface: IfaceDecl, kn_bridge_pkg_target: COutputBuffer):
        kn_bridge_pkg_target.write(
            f"  {iface.name}_impl(Kref_{iface.name} ptr):\n"
            f"    m_handle(ptr){{}};\n"
            f"\n"
        )

    def _gen_class_func_bind(
        self, func: IfaceMethodDecl, kn_bridge_pkg_target: COutputBuffer
    ):
        class_member_func_params = []
        func_impl_params_only_var = ["m_handle"]
        for param in func.params:
            type_knbridge_info = TypeKnBridgeInfo.get(self.am, param.ty_ref.resolved_ty)
            class_member_func_params.append(
                f"{type_knbridge_info.as_param} {param.name}"
            )
            func_impl_params_only_var.append(f"{param.name}")
        class_member_func_params_str = ", ".join(class_member_func_params)
        func_impl_params_only_var_str = ", ".join(func_impl_params_only_var)
        if "inner_name" not in func.attrs:
            raise KeyError("'inner_name' key not found. Exiting program.")
        konan_proj_name_list = func.attrs["inner_name"].value
        assert isinstance(konan_proj_name_list, tuple)
        konan_proj_name = konan_proj_name_list[0]
        assert isinstance(konan_proj_name, str)
        isinstance(konan_proj_name, str)
        if func.return_ty_ref is None:
            kn_bridge_pkg_target.write(
                f"  void {func.name}({class_member_func_params_str}) {{\n"
                f"    {konan_proj_name}_impl({func_impl_params_only_var_str});\n"
                f"  }}\n"
                f"\n"
            )
        else:
            return_ty_ref = func.return_ty_ref
            type_knbridge_info = TypeKnBridgeInfo.get(
                self.am, return_ty_ref.resolved_ty
            )
            return_ty_name = type_knbridge_info.as_field

            kn_bridge_pkg_target.write(
                f"  {return_ty_name} {func.name}({class_member_func_params_str}) {{\n"
                f"    {return_ty_name} result = {konan_proj_name}_impl({func_impl_params_only_var_str});\n"
                f"    return result;\n"
                f"  }}\n"
                f"\n"
            )

    def gen_func_macro(self, pkg: PackageDecl, kn_bridge_pkg_target: COutputBuffer):
        for func in pkg.functions:
            if "constructor" in func.attrs or "singleton" in func.attrs:
                kn_bridge_pkg_target.write(
                    f"TH_EXPORT_CPP_API_{func.name}({func.name}_author)\n"
                )
            else:
                konan_proj_name_list = func.attrs["inner_name"].value
                assert isinstance(konan_proj_name_list, tuple)
                konan_proj_name = konan_proj_name_list[0]
                assert isinstance(konan_proj_name, str)
                kn_bridge_pkg_target.write(
                    f"TH_EXPORT_CPP_API_{func.name}({konan_proj_name}_impl)\n"
                )

    def undef_macro(self, pkg: PackageDecl, kn_bridge_pkg_target: COutputBuffer):
        kn_bridge_pkg_target.write(
            f"#ifdef THOBJ_toKotlin\n"
            f"#undef THOBJ_toKotlin\n"
            f"#endif\n"
            f"#ifdef THCont_toKotlin\n"
            f"#undef THCont_toKotlin\n"
            f"#endif\n"
        )
