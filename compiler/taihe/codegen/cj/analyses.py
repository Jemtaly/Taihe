from taihe.semantics.declarations import (
    EnumDecl,
    GlobFuncDecl,
    IfaceDecl,
    IfaceMethodDecl,
    IfaceParentDecl,
    PackageDecl,
    PackageGroup,
    ParamDecl,
    StructDecl,
    StructFieldDecl,
    UnionDecl,
    UnionFieldDecl,
)

from taihe.semantics.types import (
    ArrayType,
    CallbackType,
    EnumType,
    IfaceType,
    MapType,
    OpaqueType,
    OptionalType,
    ScalarKind,
    ScalarType,
    SetType,
    StringType,
    StructType,
    Type,
    UnionType,
    VectorType,
)

from taihe.codegen.abi.analyses import (
    GlobFuncAbiInfo,
    IfaceAbiInfo,
    IfaceMethodAbiInfo,
    PackageAbiInfo,
    StructAbiInfo,
    TypeAbiInfo,
    UnionAbiInfo,
)
from typing_extensions import override
from abc import ABC
from taihe.semantics.visitor import TypeVisitor
from taihe.utils.analyses import AbstractAnalysis, AnalysisManager

class PackageCJInfo(AbstractAnalysis[PackageDecl]):
    def __init__(self, am: AnalysisManager, p: PackageDecl) -> None:
        self.source = f"{p.name}.cj"

    @classmethod
    @override
    def _create(cls, am: AnalysisManager, p: PackageDecl) -> "PackageCJInfo":
        return PackageCJInfo(am, p)

class TypeCJInfo(AbstractAnalysis[Type], ABC):
    defn_headers: list[str]
    impl_headers: list[str]
    # type as struct field / union field / return value
    as_owner: str
    # type as parameter
    as_param: str

    @classmethod
    @override
    def _create(cls, am: AnalysisManager, t: Type) -> "TypeCJInfo":
        return TypeCJInfoDispatcher(am).handle_type(t)

class ScalarTypeCJInfo(TypeCJInfo):
    def __init__(self, am: AnalysisManager, t: ScalarType):
        res = {
            ScalarKind.BOOL: "Bool",
            ScalarKind.F32: "Float32",
            ScalarKind.F64: "Float64",
            ScalarKind.I8: "Int8",
            ScalarKind.I16: "Int16",
            ScalarKind.I32: "Int32",
            ScalarKind.I64: "Int64",
            ScalarKind.U8: "UInt8",
            ScalarKind.U16: "UInt16",
            ScalarKind.U32: "UInt32",
            ScalarKind.U64: "UInt64",
        }.get(t.kind)
        if res is None:
            raise ValueError
        self.defn_headers = []
        self.impl_headers = []
        self.as_param = res
        self.as_owner = res

class StructTypeCJInfo(TypeCJInfo):
    def __init__(self, am: AnalysisManager, t: StructType):
        struct_abi_info = StructAbiInfo.get(am, t.ty_decl)
        self.defn_headers = []
        self.impl_headers = []
        self.as_owner = struct_abi_info.mangled_name
        self.as_param = struct_abi_info.mangled_name

class TypeCJInfoDispatcher(TypeVisitor[TypeCJInfo]):
    def __init__(self, am: AnalysisManager):
        self.am = am

    @override
    def visit_scalar_type(self, t: ScalarType) -> TypeCJInfo:
        return ScalarTypeCJInfo(self.am, t)
    
    @override
    def visit_struct_type(self, t: StructType) -> TypeCJInfo:
        return StructTypeCJInfo(self.am, t)
