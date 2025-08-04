from taihe.utils.analyses import AnalysisManager
from taihe.utils.outputs import FileKind, OutputManager

from taihe.codegen.abi.analyses import (
    GlobFuncAbiInfo,
    IfaceAbiInfo,
    IfaceMethodAbiInfo,
    PackageAbiInfo,
    StructAbiInfo,
    TypeAbiInfo,
    UnionAbiInfo,
)
from taihe.semantics.declarations import (
    GlobFuncDecl,
    IfaceDecl,
    IfaceMethodDecl,
    PackageDecl,
    PackageGroup,
    StructDecl,
    UnionDecl,
)
from taihe.codegen.cj.analyses import (
    PackageCJInfo,
    TypeCJInfo,
)
from taihe.codegen.cj.writer import CJSourceWriter

class CJCodeGenerator:
    def __init__(self, om: OutputManager, am: AnalysisManager):
        self.om = om
        self.am = am

    def generate(self, pg: PackageGroup):
        for pkg in pg.packages:
            self.gen_package_files(pkg)
    
    def gen_package_files(self, pkg: PackageDecl):
        pkg_cj_info = PackageCJInfo.get(self.am, pkg)
        with CJSourceWriter(
            self.om,
            f"cj/{pkg_cj_info.source}",
            FileKind.CJ,
        ) as pkg_cj_target:
            pkg_cj_target.writeln(f"package {pkg.name}")
            for struct in pkg.structs:
                self.gen_struct(struct, pkg_cj_target)
            for func in pkg.functions:
                self.gen_func(func, pkg_cj_target)


    def gen_func(
        self,
        func: GlobFuncDecl,
        pkg_cj_target: CJSourceWriter,
    ):
        func_abi_info = GlobFuncAbiInfo.get(self.am, func)
        params = []
        param_names = []
        for param in func.params:
            type_cj_info = TypeCJInfo.get(self.am, param.ty_ref.resolved_ty)
            params.append(f"{param.name}: {type_cj_info.as_param}")
            param_names.append(f"{param.name}")
        params_str = ", ".join(params)
        param_names_str = ", ".join(param_names)
        if return_ty_ref := func.return_ty_ref:
            type_abi_info = TypeCJInfo.get(self.am, return_ty_ref.resolved_ty)
            return_ty_name = type_abi_info.as_owner
        else:
            return_ty_name = "CPointer<Unit>"
        pkg_cj_target.writelns(
            f"foreign func {func_abi_info.mangled_name}({params_str}): {return_ty_name}",
            f"public func {func.name}({params_str}): {return_ty_name} {{",
            f"    return unsafe {{",
            f"        {func_abi_info.mangled_name}({param_names_str})",
            f"    }}",
            f"}}"
        )

    def gen_struct(
        self,
        struct: StructDecl,
        pkg_cj_target: CJSourceWriter,
    ):
        struct_abi_info = StructAbiInfo.get(self.am, struct)
        pkg_cj_target.writelns(
            f"@C",
            f"struct {struct_abi_info.mangled_name} {{",
        )
        for field in struct.fields:
            type_cj_info = TypeCJInfo.get(self.am, field.ty_ref.resolved_ty)
            pkg_cj_target.writeln(f"    let {field.name}: {type_cj_info.as_param}")
        pkg_cj_target.writeln(f"    init ()")
        pkg_cj_target.writeln(f"    let {field.name}: {type_cj_info.as_param}")
        pkg_cj_target.writeln(f"    let {field.name}: {type_cj_info.as_param}")
        pkg_cj_target.writeln(f"}}")
        pkg_cj_target.writeln(
            f"public struct {struct.name} {{",
        )
        for field in struct.fields:
            type_cj_info = TypeCJInfo.get(self.am, field.ty_ref.resolved_ty)
            pkg_cj_target.writeln(f"    public let {field.name}: {type_cj_info.as_param}")
        pkg_cj_target.writeln(f"}}")
