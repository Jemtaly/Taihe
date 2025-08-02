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
from taihe.codegen.cj.writer import CJSourceWriter
from taihe.codegen.cj.analyses import PackageCJInfo

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
            for func in pkg.functions:
                self.gen_func(func, pkg_cj_target)

    def gen_func(
        self,
        func: GlobFuncDecl,
        pkg_cj_target: CJSourceWriter,
    ):
        func_abi_info = GlobFuncAbiInfo.get(self.am, func)
        params = []
        for param in func.params:
            type_abi_info = TypeAbiInfo.get(self.am, param.ty_ref.resolved_ty)
            params.append(f"{type_abi_info.as_param} {param.name}")
        params_str = ", ".join(params)
        return_ty_name = "void"
        pkg_cj_target.writelns(
            f"package local",
            f"foreign func addI32_addI32_f(a: Int32, b: Int32): Int32",
            f"public func addI32(a: Int32, b: Int32): Int32 {{",
            f"    return unsafe {{",
            f"        addI32_addI32_f(a,b)",
            f"    }}",
            f"}}"
        )
