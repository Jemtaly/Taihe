from taihe.codegen.abi.analyses import (
    GlobFuncAbiInfo,
)
from taihe.codegen.cj.analyses import (
    PackageCJInfo,
    TypeCJInfo,
)
from taihe.codegen.cj.writer import CJSourceWriter
from taihe.semantics.declarations import (
    GlobFuncDecl,
    PackageDecl,
    PackageGroup,
    StructDecl,
)
from taihe.semantics.types import (
    StringType,
    StructType,
)
from taihe.utils.analyses import AnalysisManager
from taihe.utils.outputs import FileKind, OutputManager


class CJCodeGenerator:
    def __init__(self, om: OutputManager, am: AnalysisManager):
        self.om = om
        self.am = am
        self.TString = False

    def generate(self, pg: PackageGroup):
        for pkg in pg.packages:
            self.gen_package(pkg)

    def gen_package(self, pkg: PackageDecl):
        pkg_cj_info = PackageCJInfo.get(self.am, pkg)
        with CJSourceWriter(
            self.om,
            f"cj/{pkg_cj_info.source}",
            FileKind.CJ,
        ) as pkg_cj_target:
            pkg_cj_target.writeln(f"package {pkg.name}")
            self.gen_builtinType(pkg_cj_target)
            for struct in pkg.structs:
                self.gen_struct(struct, pkg_cj_target)
            for func in pkg.functions:
                self.gen_func(func, pkg_cj_target)

    def gen_builtinType(self, pkg_cj_target: CJSourceWriter):
        pkg_cj_target.writelns(
            f"@C",
            f"public struct TString {{",
            f"    public let flags: UInt32 = 0",
            f"    public let length: UInt32 ",
            f"    public let ptr: CString",
            f"    public TString(str: String) {{",
            f"        unsafe{{",
            f"            ptr = LibC.mallocCString(str)",
            f"            length = UInt32(str.size)",
            f"        }}",
            f"    }}",
            f"}}",
            f"",
            f"@C",
            f"public struct TArray {{",
            f"    public let m_size: IntNative",
            f"    public let m_data: CPointer<Unit> ",
            f"    public TArray(arrSize: IntNative, arrPtr: CPointer<Unit>) {{",
            f"        unsafe{{",
            f"            m_size = arrSize",
            f"            m_data = arrPtr",
            f"        }}",
            f"    }}",
            f"}}",
            f""
        )

    def gen_func(
        self,
        func: GlobFuncDecl,
        pkg_cj_target: CJSourceWriter,
    ):
        func_abi_info = GlobFuncAbiInfo.get(self.am, func)
        c_params = []
        cj_params = []
        param_names = []
        struct_mallocs = []
        struct_frees = []
        for param in func.params:
            type_cj_info = TypeCJInfo.get(self.am, param.ty_ref.resolved_ty)
            c_params.append(f"{param.name}: {type_cj_info.as_c_param}")
            cj_params.append(f"{param.name}: {type_cj_info.as_cj_param}")
            if isinstance(param.ty_ref.resolved_ty, StructType):
                struct_mallocs.append(
                    f"        let p{param.name} = LibC.malloc<{type_cj_info.as_cj_param}>()"
                )
                struct_mallocs.append(f"        p{param.name}.write({param.name})")
                struct_frees.append(f"        LibC.free(p{param.name})")
                param_names.append(f"p{param.name}")
            elif isinstance(param.ty_ref.resolved_ty, StringType):
                struct_mallocs.append(
                    f"        let middle{param.name} =TString({param.name})"
                )
                #                struct_frees.append(f"        LibC.free(p{param.name})")
                param_names.append(f"middle{param.name}")
                self.TString = True

            else:
                param_names.append(f"{param.name}")
        c_params_str = ", ".join(c_params)
        cj_params_str = ", ".join(cj_params)
        param_names_str = ", ".join(param_names)
        if return_ty_ref := func.return_ty_ref:
            type_abi_info = TypeCJInfo.get(self.am, return_ty_ref.resolved_ty)
            return_c_ty_name = type_abi_info.as_c_owner
            return_cj_ty_name = type_abi_info.as_cj_owner
        else:
            return_cj_ty_name = "Unit"
        pkg_cj_target.writelns(
            f"foreign func {func_abi_info.mangled_name}({c_params_str}): {return_c_ty_name}",
            f"public func {func.name}({cj_params_str}): {return_cj_ty_name} {{",
            f"    unsafe {{",
        )
        for struct_malloc in struct_mallocs:
            pkg_cj_target.writeln(struct_malloc)
        if return_cj_ty_name != "String":
            pkg_cj_target.writeln(
                f"        let res = {func_abi_info.mangled_name}({param_names_str})"
            )
        else:
            pkg_cj_target.writeln(
                f"        let res = {func_abi_info.mangled_name}({param_names_str}).ptr.toString()"
            )
        for struct_free in struct_frees:
            pkg_cj_target.writeln(struct_free)
        pkg_cj_target.writelns(
            f"        return res",
            f"    }}",
            f"}}",
        )

    def gen_struct(self, struct: StructDecl, pkg_cj_target: CJSourceWriter):
        pkg_cj_target.writelns(f"@C", f"public struct {struct.name} {{")
        paramsInit = []
        param_name = []
        str_param_name = []
        for field in struct.fields:
            type_cj_info = TypeCJInfo.get(self.am, field.ty_ref.resolved_ty)
            paramsInit.append(f"{field.name}:{type_cj_info.as_cj_param}")
            if isinstance(field.ty_ref.resolved_ty, StringType):
                pkg_cj_target.writeln(
                    f"    let {field.name}: {type_cj_info.as_c_param}"
                )
                str_param_name.append(f"{field.name}")
            else:
                pkg_cj_target.writeln(
                    f"public let {field.name}: {type_cj_info.as_c_param}"
                )
                param_name.append(f"{field.name}")
        params_str = ", ".join(paramsInit)
        pkg_cj_target.writeln(f"    public {struct.name} ({params_str}){{")
        for name in param_name:
            pkg_cj_target.writeln(f"        this.{name}={name}")
        for name in str_param_name:
            pkg_cj_target.writeln(f"        this.{name}=TString({name})")
        pkg_cj_target.writeln(f"    }}")
        pkg_cj_target.writeln(f"    ")
        pkg_cj_target.writeln(f"}}")
