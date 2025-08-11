from abc import ABC

from typing_extensions import override

from taihe.semantics.declarations import (
    PackageDecl,
)
from taihe.semantics.types import (
    ScalarKind,
    ScalarType,
    StructType,
    Type,
    StringType,
)

from taihe.semantics.visitor import TypeVisitor
from taihe.utils.analyses import AbstractAnalysis, AnalysisManager

from taihe.codegen.cj.writer import CJSourceWriter

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
    as_c_owner: str
    # type as parameter
    as_c_param: str
    as_cj_owner: str
    as_cj_param: str

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
        self.as_c_param = res
        self.as_c_owner = res
        self.as_cj_owner = res
        self.as_cj_param = res


class StructTypeCJInfo(TypeCJInfo):
    def __init__(self, am: AnalysisManager, t: StructType):
        self.defn_headers = []
        self.impl_headers = []
        self.as_c_owner = t.ty_decl.name
        self.as_c_param = "CPointer<" + t.ty_decl.name + ">"
        self.as_cj_owner = t.ty_decl.name
        self.as_cj_param = t.ty_decl.name
class StringTypeCJInfo(TypeCJInfo):
    def __init__(self, am: AnalysisManager, t: StringType):
        self.defn_headers = []
        self.impl_headers = []
        self.as_c_owner = "TString"
        self.as_c_param = "TString"
        self.as_cj_owner = "String"
        self.as_cj_param = "String"
class TypeCJInfoDispatcher(TypeVisitor[TypeCJInfo]):    
    def __init__(self, am: AnalysisManager):
        self.am = am

    @override
    def visit_scalar_type(self, t: ScalarType) -> TypeCJInfo:
        return ScalarTypeCJInfo(self.am, t)

    @override
    def visit_struct_type(self, t: StructType) -> TypeCJInfo:
        return StructTypeCJInfo(self.am, t)
    
    @override
    def visit_string_type(self, t: StringType) -> TypeCJInfo:
        return StringTypeCJInfo(self.am, t)