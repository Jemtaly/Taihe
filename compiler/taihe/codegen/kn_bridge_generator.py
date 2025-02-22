from abc import ABCMeta

from typing_extensions import override

from taihe.semantics.declarations import (
    GlobFuncDecl,
    IfaceDecl,
    IfaceMethodDecl,
    Package,
    PackageGroup,
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
    # BoxType,
    # CallbackType,
    # EnumType,
    IfaceType,
    # MapType,
    ScalarType,
    # SetType,
    SpecialType,
    # StructType,
    Type,
    # VectorType,
)
from taihe.semantics.visitor import TypeVisitor
from taihe.utils.analyses import AbstractAnalysis, AnalysisManager
from taihe.utils.outputs import COutputBuffer, OutputManager


class KNBridgePackageInfo(AbstractAnalysis[Package]):
    def __init__(self, am: AnalysisManager, p: Package) -> None:
        # self.header = f"{p.name}.api.h"
        self.source = f"{p.name}.api.cpp"
        self.include_header = f"{p.name}.impl.cpp"


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
    as_field: str
    as_param: str
    as_konan_param: str
    as_konan_field: str
    need_holder: bool
    param_covert_func: str
    retval_convert_func: str

    # def return_from_abi(self, val):
    #     return f"::taihe::core::from_abi<{self.as_field}>({val})"

    # def return_into_abi(self, val):
    #     return f"::taihe::core::into_abi<{self.as_field}>({val})"

    # def pass_from_abi(self, val):
    #     return f"::taihe::core::from_abi<{self.as_param}>({val})"

    # def pass_into_abi(self, val):
    #     return f"::taihe::core::into_abi<{self.as_param}>({val})"


class IfaceDeclKnBridgeInfo(AbstractAnalysis[IfaceDecl]):
    def __init__(self, am: AnalysisManager, d: IfaceDecl) -> None:
        p = d.node_parent
        assert p
        self.name = d.name
        self.as_owner = self.name
        self.as_param = self.name
        self.as_field = self.name
        self.as_konan_param = self.name
        self.as_konan_field = self.name
        temp = d.attrs["type_function"].value
        # assert isinstance(temp, list)
        # self.type_function = temp[0]
        # assert isinstance(self.type_function, str)
        self.type_function = temp


class IfaceTypeKnBridgeInfo(AbstractAnalysis[IfaceType], AbstractTypeKnBridgeInfo):
    def __init__(self, am: AnalysisManager, t: IfaceType):
        iface_knbridge_info = IfaceDeclKnBridgeInfo.get(am, t.ty_decl)
        #  need to do
        self.decl_headers = []
        self.defn_headers = []
        self.as_field = iface_knbridge_info.as_field
        self.as_param = iface_knbridge_info.as_param
        self.as_konan_param = iface_knbridge_info.as_konan_param
        self.as_konan_field = iface_knbridge_info.as_konan_field
        self.param_covert_func: str = ""
        self.retval_convert_func: str = ""
        self.need_holder = True


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
        self.as_param = res
        self.as_field = res
        self.as_konan_param = res
        self.as_konan_field = res
        self.param_covert_func = None
        self.retval_convert_func = None
        self.need_holder = False


class SpecialTypeKnBridgeInfo(AbstractAnalysis[SpecialType], AbstractTypeKnBridgeInfo):
    def __init__(self, am: AnalysisManager, t: SpecialType):
        if t != STRING:
            raise ValueError
        self.decl_headers = ["core/string.hpp"]
        self.defn_headers = ["core/string.hpp"]
        self.as_field = "::taihe::core::string"
        self.as_param = "::taihe::core::string_view"
        self.as_konan_param = "KObjHeader*"
        self.as_konan_field = "KObjHeader*"
        self.param_covert_func = "CreateStringFromTHString"
        self.retval_convert_func = "CreateTHStringFromString"
        self.need_holder = True


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

    def generate(self, pg: PackageGroup):
        for pkg in pg.packages:
            # self.gen_package_header_file(pkg)
            self.gen_package_source_file(pkg)

    # def gen_package_header_file(self, pkg: Package):
    #     kn_bridge_pkg_info = KNBridgePackageInfo.get(self.am, pkg)
    #     kn_bridge_pkg_target = COutputBuffer.create(
    #         self.tm, f"{kn_bridge_pkg_info.header}", True
    #     )

    #     temp = pkg.attrs["prefix"].value
    #     # assert isinstance(temp, list)
    #     # kn_bridge_prefix = temp[0]
    #     # assert isinstance(kn_bridge_prefix, str)
    #     kn_bridge_prefix = temp

    #     # self.gen_package_th_tydef(pkg, kn_bridge_pkg_target, kn_bridge_prefix)

    #     # self.gen_package_kn_typedef(kn_bridge_pkg_target, kn_bridge_prefix)

    #     # self.gen_typedef_struct(pkg, kn_bridge_pkg_target, kn_bridge_prefix)

    #     # self.gen_struct_above(kn_bridge_pkg_target, kn_bridge_prefix)

    #     # self.gen_struct_func(pkg, kn_bridge_pkg_target, kn_bridge_prefix)

    #     # self.gen_struct_below(kn_bridge_pkg_target, kn_bridge_prefix)

    #     # self.gen_package_th_tyundef(pkg, kn_bridge_pkg_target)

    def gen_package_source_file(self, pkg: Package):
        kn_bridge_pkg_info = KNBridgePackageInfo.get(self.am, pkg)
        kn_bridge_pkg_target = COutputBuffer.create(
            self.tm, f"{kn_bridge_pkg_info.source}", False
        )

        temp = pkg.attrs["prefix"].value
        # assert isinstance(temp, list)
        # kn_bridge_prefix = temp[0]
        # assert isinstance(kn_bridge_prefix, str)
        kn_bridge_prefix = temp

        kn_bridge_pkg_target.include("core/string.hpp")
        kn_bridge_pkg_target.include(kn_bridge_pkg_info.include_header)
        kn_bridge_pkg_target.write(f"#include <stdint.h>\n")

        self.gen_func_impl(pkg, kn_bridge_pkg_target, kn_bridge_prefix)
        self.gen_toplevel_func_macro(pkg, kn_bridge_pkg_target)

    def gen_package_th_tydef(self, pkg: Package, kn_bridge_pkg_target: COutputBuffer):
        for iface in pkg.interfaces:
            kn_bridge_pkg_target.write(f"typedef void* Kref_{iface.name}\n" f"\n")

    def gen_need_decl(self, pkg: Package, kn_bridge_pkg_target: COutputBuffer):
        kn_bridge_pkg_target.write(
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
        )

    def gen_func_impl(
        self, pkg: Package, kn_bridge_pkg_target: COutputBuffer, kn_bridge_prefix: str
    ):
        self.gen_iface_method_impl(pkg, kn_bridge_pkg_target, kn_bridge_prefix)
        self.gen_toplevel_func_impl(pkg, kn_bridge_pkg_target)

    def gen_iface_method_impl(
        self, pkg: Package, kn_bridge_pkg_target: COutputBuffer, kn_bridge_prefix: str
    ):
        type_func_name = None
        for iface in pkg.interfaces:
            init_func_fin = False
            iface_info = IfaceDeclKnBridgeInfo.get(self.am, iface)
            type_func_name = iface_info.type_function
            kn_bridge_pkg_target.write(
                f'extern "C" {kn_bridge_prefix}_KType* {type_func_name}(void);\n'
            )
            for func in iface.methods:
                # kn_bridge_func_info = GlobFuncDeclKnBridgeInfo.get(self.am, func)
                # need to abstract to a class
                params_holder = []
                params = []
                konan_params_only_ty = []
                convert_params = []
                konan_proj_name = None
                need_ret_holder = False

                temp = func.attrs["inner_name"].value
                # assert isinstance(temp, list)
                # konan_proj_name = temp[0]
                # assert isinstance(konan_proj_name, str)
                konan_proj_name = temp

                for param in func.params:
                    type_knbridge_info = TypeKnBridgeInfo.get(
                        self.am, param.ty_ref.resolved_ty
                    )
                    params.append(f"{type_knbridge_info.as_param} {param.name}")
                    params_holder.append(type_knbridge_info.need_holder)
                    konan_params_only_ty.append(f"{type_knbridge_info.as_konan_param}")
                    if type_knbridge_info.need_holder:
                        convert_params.append(
                            f"{type_knbridge_info.param_covert_func}({param.name}, {param.name}_holder.slot())"
                        )
                    else:
                        convert_params.append(f"{param.name}")
                params_str = ", ".join(params)
                konan_params_only_ty_str = ", ".join(konan_params_only_ty)
                convert_params_str = ", ".join(convert_params)
                print(konan_params_only_ty_str)

                return_ty_name = ""
                return_ty_str = ""
                return_ty_konan_name = ""

                if func.return_ty_ref is None:
                    return_ty_name = "void"
                    return_ty_str = ""

                    return_ty_konan_name = "void"
                elif return_ty_ref := func.return_ty_ref:
                    type_knbridge_info = TypeKnBridgeInfo.get(
                        self.am, return_ty_ref.resolved_ty
                    )
                    need_ret_holder = type_knbridge_info.need_holder
                    return_ty_name = type_knbridge_info.as_field
                    if need_ret_holder:
                        return_ty_str = (
                            f"{type_knbridge_info.retval_convert_func}(result)"
                        )

                        konan_params_only_ty.append(f"KObjHeader**")
                        convert_params.append("result_holder.slot()")
                    else:
                        return_ty_str = "result"

                    return_ty_konan_name = type_knbridge_info.as_konan_field
                # func generate
                if func.name == "init":
                    init_func_fin = True
                    return_ty_konan_name = "void"
                kn_bridge_pkg_target.write(
                    f'extern "C" {return_ty_konan_name} {konan_proj_name}({konan_params_only_ty_str});\n'
                    f"static {return_ty_name} {func.name}Impl({params_str}) {{\n"
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
                if func.return_ty_ref is None:
                    kn_bridge_pkg_target.write(
                        f"    {konan_proj_name}({convert_params_str});\n"
                    )
                elif init_func_fin:
                    kn_bridge_pkg_target.write(
                        f"    KObjHolder result_holder;\n"
                        f"    KObjHeader* result = AllocInstance((const KTypeInfo*){type_func_name}(), result_holder.slot());\n"
                        f"    {konan_proj_name}({convert_params_str});\n"
                        f"    return (({return_ty_name}){{ .pinned = CreateStablePointer(result)}});\n"
                    )
                    init_func_fin = False
                else:
                    kn_bridge_pkg_target.write(
                        f"    KObjHolder result_holder;\n"
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

    def gen_toplevel_func_impl(self, pkg: Package, kn_bridge_pkg_target: COutputBuffer):
        for func in pkg.functions:
            # kn_bridge_func_info = GlobFuncDeclKnBridgeInfo.get(self.am, func)
            # need to abstract to a class
            params_holder = []
            params = []
            konan_params_only_ty = []
            convert_params = []
            konan_proj_name = None
            need_ret_holder = False

            temp = func.attrs["inner_name"].value
            # assert isinstance(temp, list)
            # konan_proj_name = temp[0]
            # assert isinstance(konan_proj_name, str)
            konan_proj_name = temp

            for param in func.params:
                type_knbridge_info = TypeKnBridgeInfo.get(
                    self.am, param.ty_ref.resolved_ty
                )
                params.append(f"{type_knbridge_info.as_param} {param.name}")
                params_holder.append(type_knbridge_info.need_holder)
                konan_params_only_ty.append(f"{type_knbridge_info.as_konan_param}")
                if type_knbridge_info.need_holder:
                    convert_params.append(
                        f"{type_knbridge_info.param_covert_func}({param.name}, {param.name}_holder.slot())"
                    )
                else:
                    convert_params.append(f"{param.name}")

            return_ty_name = ""
            return_ty_str = ""
            return_ty_konan_name = ""

            if func.return_ty_ref is None:
                return_ty_name = "void"
                return_ty_str = ""

                return_ty_konan_name = "void"
            elif return_ty_ref := func.return_ty_ref:
                type_knbridge_info = TypeKnBridgeInfo.get(
                    self.am, return_ty_ref.resolved_ty
                )
                need_ret_holder = type_knbridge_info.need_holder
                return_ty_name = type_knbridge_info.as_field
                if need_ret_holder:
                    return_ty_str = f"{type_knbridge_info.retval_convert_func}(result)"

                    konan_params_only_ty.append(f"KObjHeader**")
                    convert_params.append("result_holder.slot()")

                else:
                    return_ty_str = "result"

                return_ty_konan_name = type_knbridge_info.as_konan_field

            params_str = ", ".join(params)
            konan_params_only_ty_str = ", ".join(konan_params_only_ty)
            convert_params_str = ", ".join(convert_params)

            # fun generate
            kn_bridge_pkg_target.write(
                f'extern "C" {return_ty_konan_name} {konan_proj_name} ({konan_params_only_ty_str});\n'
                f"static {return_ty_name} {func.name}Impl({params_str}) {{\n"
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

    def gen_toplevel_func_macro(
        self, pkg: Package, kn_bridge_pkg_target: COutputBuffer
    ):
        for func in pkg.functions:
            kn_bridge_pkg_target.write(
                f"TH_EXPORT_CPP_API_{func.name}({func.name}Impl)\n"
            )
