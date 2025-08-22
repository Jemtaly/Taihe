from taihe.codegen.abi.analyses import (
    GlobFuncAbiInfo,
)
from taihe.codegen.cj.analyses import (
    PackageCJInfo,
    TypeCJInfo,
)
from taihe.codegen.cj.writer import CJSourceWriter
from taihe.semantics.declarations import (
    EnumDecl,
    GlobFuncDecl,
    PackageDecl,
    PackageGroup,
    StructDecl,
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
            for enum in pkg.enums:
                self.gen_enum(enum, pkg_cj_target)
            for func in pkg.functions:
                self.gen_func(func, pkg_cj_target)

    def gen_builtinType(self, pkg_cj_target: CJSourceWriter):
        pkg_cj_target.writelns(
            f"@C",
            f"public struct TString {{",
            f"    public let flags: UInt32 = 0",
            f"    public let length: UInt32",
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
            f"",
            f"@C",
            f"public struct TOptional {{",
            f"    public let m_data: CPointer<Unit>",
            f"    public TOptional(data: CPointer<Unit>) {{",
            f"        unsafe{{",
            f"            m_data = data",
            f"        }}",
            f"    }}",
            f"}}",
            f"",
            f"@C",
            f"struct IdMapItem {{",
            f"    let id: CPointer<Unit>",
            f"    let vtbl_ptr: CPointer<Unit>",
            f"    public IdMapItem(p1: CPointer<Unit>, p2: CPointer<Unit>) {{",
            f"        id = p1",
            f"        vtbl_ptr = p2",
            f"    }}",
            f"}}",
            f"",
            f"@C",
            f"struct TypeInfo {{",
            f"    let version: UInt64",
            f"    let free_fptr: CFunc<(CPointer<DataBlockHead>)->Unit>",
            f"    let hash_fptr: CFunc<(CPointer<DataBlockHead>)->UIntNative>",
            f"    let same_fptr: CFunc<(CPointer<DataBlockHead>, CPointer<DataBlockHead>)->Bool>",
            f"    let len: UInt64",
            f"    let Idmap: CPointer<IdMapItem>",
            f"    public TypeInfo(p1: UInt64,",
            f"        p2: CFunc<(CPointer<DataBlockHead>)->Unit>,",
            f"        p3: CFunc<(CPointer<DataBlockHead>)->UIntNative>,",
            f"        p4: CFunc<(CPointer<DataBlockHead>, CPointer<DataBlockHead>)->Bool>,",
            f"        p5: UInt64,",
            f"        p6: CPointer<IdMapItem>) {{",
            f"        version = p1",
            f"        free_fptr = p2",
            f"        hash_fptr = p3",
            f"        same_fptr = p4",
            f"        len = p5",
            f"        Idmap = p6",
            f"    }}",
            f"}}",
            f"",
            f"@C",
            f"struct DataBlockHead {{",
            f"    var rtti_ptr: CPointer<TypeInfo>",
            f"    let m_count: UInt32",
            f"    public DataBlockHead(p1: CPointer<TypeInfo>, p2: UInt32) {{",
            f"        rtti_ptr = p1",
            f"        m_count = p2",
            f"    }}",
            f"}}",
            f"",
        )

    def gen_func(
        self,
        func: GlobFuncDecl,
        pkg_cj_target: CJSourceWriter,
    ):
        func_abi_info = GlobFuncAbiInfo.get(self.am, func)
        c_params = []
        cj_params = []
        for param in func.params:
            type_cj_info = TypeCJInfo.get(self.am, param.ty_ref.resolved_ty)
            c_params.append(f"{param.name}: {type_cj_info.as_c_param}")
            cj_params.append(f"{param.name}: {type_cj_info.as_cj_param}")
        c_params_str = ", ".join(c_params)
        cj_params_str = ", ".join(cj_params)
        if return_ty_ref := func.return_ty_ref:
            type_cj_info = TypeCJInfo.get(self.am, return_ty_ref.resolved_ty)
            return_c_ty_name = type_cj_info.as_c_owner
            return_cj_ty_name = type_cj_info.as_cj_owner
        else:
            return_c_ty_name = "Unit"
            return_cj_ty_name = "Unit"
        pkg_cj_target.writelns(
            f"foreign func {func_abi_info.mangled_name}({c_params_str}): {return_c_ty_name}",
            f"public func {func.name}({cj_params_str}): {return_cj_ty_name} {{",
            f"    unsafe {{",
        )
        param_names = []
        for param in func.params:
            type_cj_info = TypeCJInfo.get(self.am, param.ty_ref.resolved_ty)
            param_cj_name = type_cj_info.from_cj(
                pkg_cj_target, param.name, type_cj_info.as_cj_owner
            )
            param_names.append(f"{param_cj_name}")
        param_names_str = ", ".join(param_names)
        pkg_cj_target.writeln(
            f"        let cRes = {func_abi_info.mangled_name}({param_names_str})"
        )
        if return_ty_ref := func.return_ty_ref:
            type_abi_info = TypeCJInfo.get(self.am, return_ty_ref.resolved_ty)
            type_abi_info.into_cj(pkg_cj_target)
        else:
            pkg_cj_target.writeln(f"        let cjRes = cRes")
        for param in func.params:
            type_cj_info = TypeCJInfo.get(self.am, param.ty_ref.resolved_ty)
            type_cj_info.free(pkg_cj_target, param.name, type_cj_info.as_cj_owner)
        pkg_cj_target.writelns(f"        return cjRes", f"    }}", f"}}", f"")

    def gen_struct(self, struct: StructDecl, pkg_cj_target: CJSourceWriter):
        pkg_cj_target.writelns(f"@C", f"public struct {struct.name} {{")
        paramsInit = []
        param_name = []
        str_param_name = []
        for field in struct.fields:
            type_cj_info = TypeCJInfo.get(self.am, field.ty_ref.resolved_ty)
            paramsInit.append(f"{field.name}:{type_cj_info.as_cj_param}")
            pkg_cj_target.writeln(f"public let {field.name}: {type_cj_info.as_c_param}")
            param_name.append(f"{field.name}")
        params_str = ", ".join(paramsInit)
        pkg_cj_target.writeln(f"    public {struct.name} ({params_str}){{")
        for field in struct.fields:
            type_cj_info = TypeCJInfo.get(self.am, field.ty_ref.resolved_ty)
            param_cj_name = type_cj_info.from_cj(
                pkg_cj_target, field.name, type_cj_info.as_cj_owner
            )
            pkg_cj_target.writeln(f"        this.{field.name}={param_cj_name}")
        pkg_cj_target.writelns(f"    }}", f"    ", f"}}")

    def gen_enum(self, enum: EnumDecl, pkg_cj_target: CJSourceWriter):
        pkg_cj_target.writeln(f"public enum {enum.name} {{")
        for ctor in enum.items:
            pkg_cj_target.writeln(f"    | {ctor.name}")
        pkg_cj_target.writeln(f"")
        pkg_cj_target.writelns(
            f"    public func getIdx() : Int32 {{", f"        match(this) {{"
        )
        for idx, ctor in enumerate(enum.items):
            pkg_cj_target.writeln(f"            case {ctor.name} => {idx}")
        pkg_cj_target.writelns(
            f'            case _ => throw Exception("illegal enum value")',
            f"        }}",
            f"    }}",
            f"",
        )
        pkg_cj_target.writelns(
            f"    static func parse(val: Int32) : {enum.name} {{",
            f"        match(val) {{",
        )
        for idx, ctor in enumerate(enum.items):
            pkg_cj_target.writeln(f"            case {idx} => {ctor.name}")
        pkg_cj_target.writelns(
            f'            case _ => throw Exception("illegal enum value")',
            f"        }}",
            f"    }}",
            f"",
        )
        pkg_cj_target.writelns(
            f"    public func toString() : String {{", f"        match(this) {{"
        )
        for ctor in enum.items:
            pkg_cj_target.writeln(f'            case {ctor.name} => "{ctor.value}"')
        pkg_cj_target.writelns(
            f'            case _ => throw Exception("illegal enum value")',
            f"        }}",
            f"    }}",
            f"}}",
            f"",
        )
