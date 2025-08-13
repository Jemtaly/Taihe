from abc import ABC, abstractmethod

from typing_extensions import override

from taihe.codegen.abi.analyses import EnumAbiInfo
from taihe.codegen.cj.writer import CJSourceWriter
from taihe.semantics.declarations import (
    PackageDecl,
)
from taihe.semantics.types import (
    ArrayType,
    CallbackType,
    EnumType,
    OptionalType,
    ScalarKind,
    ScalarType,
    StringType,
    StructType,
    Type,
    UnionType,
)
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
    # type as struct field / union field / return value in @C/foreign function
    as_c_owner: str
    # type as parameter in @C/foreign function
    as_c_param: str
    # type as struct field / union field / return value in CJ function
    as_cj_owner: str
    # type as parameter in CJ function
    as_cj_param: str

    @classmethod
    @override
    def _create(cls, am: AnalysisManager, t: Type) -> "TypeCJInfo":
        return TypeCJInfoDispatcher(am).handle_type(t)

    @abstractmethod
    def from_cj(
        self,
        target: CJSourceWriter,
        abi_type: str,
        cj_type: str,
    ):
        pass

    @abstractmethod
    def into_cj(
        self,
        target: CJSourceWriter,
        abi_type: str,
        cj_type: str,
    ):
        pass


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

    def from_cj(
        self,
        target: CJSourceWriter,
        abi_type: str,
        cj_type: str,
    ):
        pass

    def into_cj(
        self,
        target: CJSourceWriter,
        abi_type: str,
        cj_type: str,
    ):
        pass


class StructTypeCJInfo(TypeCJInfo):
    def __init__(self, am: AnalysisManager, t: StructType):
        self.defn_headers = []
        self.impl_headers = []
        self.as_c_owner = t.ty_decl.name
        self.as_c_param = "CPointer<" + t.ty_decl.name + ">"
        self.as_cj_owner = t.ty_decl.name
        self.as_cj_param = t.ty_decl.name

    def from_cj(
        self,
        target: CJSourceWriter,
        abi_type: str,
        cj_type: str,
    ):
        pass

    def into_cj(
        self,
        target: CJSourceWriter,
        abi_type: str,
        cj_type: str,
    ):
        pass


class StringTypeCJInfo(TypeCJInfo):
    def __init__(self, am: AnalysisManager, t: StringType):
        self.defn_headers = []
        self.impl_headers = []
        self.as_c_owner = "TString"
        self.as_c_param = "TString"
        self.as_cj_owner = "String"
        self.as_cj_param = "String"

    def from_cj(
        self,
        target: CJSourceWriter,
        abi_type: str,
        cj_type: str,
    ):
        pass

    def into_cj(
        self,
        target: CJSourceWriter,
        abi_type: str,
        cj_type: str,
    ):
        pass


class ArrayTypeCJInfo(TypeCJInfo):
    def __init__(self, am: AnalysisManager, t: ArrayType):
        self.defn_headers = []
        self.impl_headers = []
        arg_ty_abi_info = TypeCJInfo.get(am, t.item_ty)
        self.as_c_owner = "VArray<" + arg_ty_abi_info.as_cj_param + ">"
        self.as_c_param = "VArray<" + arg_ty_abi_info.as_cj_param + ">"
        self.as_cj_owner = "VArray<" + arg_ty_abi_info.as_cj_param + ">"
        self.as_cj_param = "VArray<" + arg_ty_abi_info.as_cj_param + ">"

    def from_cj(
        self,
        target: CJSourceWriter,
        abi_type: str,
        cj_type: str,
    ):
        pass

    def into_cj(
        self,
        target: CJSourceWriter,
        abi_type: str,
        cj_type: str,
    ):
        pass


class EnumTypeCJInfo(TypeCJInfo):
    def __init__(self, am: AnalysisManager, t: EnumType):
        self.defn_headers = []
        self.impl_headers = []
        self.as_c_owner = "UInt32"
        self.as_c_param = "UInt32"
        enum_abi_info = EnumAbiInfo.get(am, t.ty_decl)
        self.as_cj_owner = enum_abi_info.abi_type
        self.as_cj_param = enum_abi_info.abi_type

    def from_cj(
        self,
        target: CJSourceWriter,
        abi_type: str,
        cj_type: str,
    ):
        pass

    def into_cj(
        self,
        target: CJSourceWriter,
        abi_type: str,
        cj_type: str,
    ):
        pass


class OptionalTypeCJInfo(TypeCJInfo):
    def __init__(self, am: AnalysisManager, t: OptionalType):
        self.defn_headers = []
        self.impl_headers = []
        self.as_c_owner = "TOptional"
        self.as_c_param = "TOptional"
        self.as_cj_owner = "TOptional"
        self.as_cj_param = "TOptional"

    def from_cj(
        self,
        target: CJSourceWriter,
        abi_type: str,
        cj_type: str,
    ):
        pass

    def into_cj(
        self,
        target: CJSourceWriter,
        abi_type: str,
        cj_type: str,
    ):
        pass


class UnionTypeCJInfo(TypeCJInfo):
    def __init__(self, am: AnalysisManager, t: UnionType):
        self.defn_headers = []
        self.impl_headers = []
        self.as_c_owner = "Union"
        self.as_c_param = "Union"
        self.as_cj_owner = "Union"
        self.as_cj_param = "Union"

    def from_cj(
        self,
        target: CJSourceWriter,
        abi_type: str,
        cj_type: str,
    ):
        pass

    def into_cj(
        self,
        target: CJSourceWriter,
        abi_type: str,
        cj_type: str,
    ):
        pass

class CallbackTypeCJInfo(TypeCJInfo):
    def __init__(self, am: AnalysisManager, t: CallbackType):
        self.defn_headers = []
        self.impl_headers = []
        self.as_c_owner = "LAMBDA"
        self.as_c_param = "LAMBDA"
        self.as_cj_owner = "LAMBDA"
        self.as_cj_param = "LAMBDA"

    def from_cj(
        self,
        target: CJSourceWriter,
        abi_type: str,
        cj_type: str,
    ):
        pass

    def into_cj(
        self,
        target: CJSourceWriter,
        abi_type: str,
        cj_type: str,
    ):
        pass


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

    @override
    def visit_array_type(self, t: ArrayType) -> TypeCJInfo:
        return ArrayTypeCJInfo(self.am, t)

    @override
    def visit_enum_type(self, t: EnumType) -> TypeCJInfo:
        return EnumTypeCJInfo(self.am, t)

    @override
    def visit_optional_type(self, t: OptionalType) -> TypeCJInfo:
        return OptionalTypeCJInfo(self.am, t)

    @override
    def visit_union_type(self, t: UnionType) -> TypeCJInfo:
        return UnionTypeCJInfo(self.am, t)
    
    @override
    def visit_callback_type(self, t: CallbackType):
        return CallbackTypeCJInfo(self.am, t)
