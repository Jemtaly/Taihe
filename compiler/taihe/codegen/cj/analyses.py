from abc import ABC, abstractmethod

from typing_extensions import override

from taihe.codegen.cj.writer import CJSourceWriter
from taihe.semantics.declarations import (
    PackageDecl,
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
    def from_cj(self, target: CJSourceWriter, name: str, cj_type: str) -> str:
        pass

    @abstractmethod
    def into_cj(
        self,
        target: CJSourceWriter,
    ):
        pass

    @abstractmethod
    def free(
        self,
        target: CJSourceWriter,
        name: str,
        cj_type: str,
    ):
        pass


class EnumTypeCJInfo(TypeCJInfo):
    def __init__(self, am: AnalysisManager, t: EnumType):
        self.t = t
        self.defn_headers = []
        self.impl_headers = []
        self.as_c_owner = "Int32"
        self.as_c_param = "Int32"
        self.as_cj_owner = t.ty_decl.name
        self.as_cj_param = t.ty_decl.name

    def from_cj(
        self,
        target: CJSourceWriter,
        name: str,
        cj_type: str,
    ) -> str:
        target.writeln(f"        let idx = {name}.getIdx()")
        return "idx"

    def into_cj(
        self,
        target: CJSourceWriter,
    ):
        target.writeln(f"        let cjRes = {self.t.ty_decl.name}.parse(cRes)")

    def free(
        self,
        target: CJSourceWriter,
        name: str,
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

    def from_cj(self, target: CJSourceWriter, name: str, cj_type: str) -> str:
        return name

    def into_cj(
        self,
        target: CJSourceWriter,
    ):
        target.writeln(f"        let cjRes = cRes")

    def free(
        self,
        target: CJSourceWriter,
        name: str,
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

    def from_cj(self, target: CJSourceWriter, name: str, cj_type: str) -> str:
        target.writelns(
            f"        let p{name} = LibC.malloc<{cj_type}>()",
            f"        p{name}.write({name})",
        )
        return f"p{name}"

    def into_cj(
        self,
        target: CJSourceWriter,
    ):
        target.writeln(f"        let cjRes = cRes")

    def free(
        self,
        target: CJSourceWriter,
        name: str,
        cj_type: str,
    ):
        target.writelns(f"        LibC.free(p{name})")


class IfaceTypeCJInfo(TypeCJInfo):
    def __init__(self, am: AnalysisManager, t: IfaceType):
        self.defn_headers = []
        self.impl_headers = []
        self.as_c_owner = t.ty_decl.name
        self.as_c_param = "CPointer<" + t.ty_decl.name + ">"
        self.as_cj_owner = t.ty_decl.name
        self.as_cj_param = t.ty_decl.name

    def from_cj(self, target: CJSourceWriter, name: str, cj_type: str) -> str:
        return name

    def into_cj(
        self,
        target: CJSourceWriter,
    ):
        target.writeln(f"        let cjRes = cRes")

    def free(
        self,
        target: CJSourceWriter,
        name: str,
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

    def from_cj(self, target: CJSourceWriter, name: str, cj_type: str) -> str:
        return name

    def into_cj(
        self,
        target: CJSourceWriter,
    ):
        target.writeln(f"        let cjRes = cRes")

    def free(
        self,
        target: CJSourceWriter,
        name: str,
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

    def from_cj(self, target: CJSourceWriter, name: str, cj_type: str) -> str:
        target.writelns(
            f"        let middle{name} = TString({name})",
        )
        return f"middle{name}"

    def into_cj(
        self,
        target: CJSourceWriter,
    ):
        target.writeln(f"        let cjRes = cRes.ptr.toString()")

    def free(
        self,
        target: CJSourceWriter,
        name: str,
        cj_type: str,
    ):
        pass


class ArrayTypeCJInfo(TypeCJInfo):
    def __init__(self, am: AnalysisManager, t: ArrayType):
        self.defn_headers = []
        self.impl_headers = []
        self.arg_ty_info = TypeCJInfo.get(am, t.item_ty)
        self.as_c_owner = "TArray"
        self.as_c_param = "TArray"
        self.as_cj_owner = "Array<" + self.arg_ty_info.as_cj_param + ">"
        self.as_cj_param = "Array<" + self.arg_ty_info.as_cj_param + ">"

    def from_cj(self, target: CJSourceWriter, name: str, cj_type: str) -> str:
        target.writelns(
            f"        let temp1_{name}: CPointerHandle<{self.arg_ty_info.as_cj_param}> = acquireArrayRawData({name})",
            f"        let temp2_{name} = temp1_{name}.pointer",
            f"        let temp3_{name} = TArray(IntNative({name}.size), CPointer<Unit>(temp2_{name}))",
        )
        return f"temp3_{name}"

    def into_cj(
        self,
        target: CJSourceWriter,
    ):
        target.writelns(
            f"        var cjRes = Array<{self.arg_ty_info.as_cj_param}>(Int64(cRes.m_size), repeat: zeroValue<{self.arg_ty_info.as_cj_param}>())",
            f"        for (i in 0..Int64(cRes.m_size)) {{",
            f"            cjRes[i] = CPointer<{self.arg_ty_info.as_cj_param}>(cRes.m_data).read(i)",
            f"        }}",
        )

    def free(
        self,
        target: CJSourceWriter,
        name: str,
        cj_type: str,
    ):
        target.writeln(
            f"        releaseArrayRawData<{self.arg_ty_info.as_cj_param}>(temp1_{name})"
        )


class OptionalTypeCJInfo(TypeCJInfo):
    def __init__(self, am: AnalysisManager, t: OptionalType):
        self.am = am
        self.t = t
        self.defn_headers = []
        self.impl_headers = []
        self.option_arg_info = TypeCJInfo.get(am, t.item_ty)
        self.as_c_owner = "TOptional"
        self.as_c_param = "TOptional"
        self.as_cj_owner = "Option<" + self.option_arg_info.as_cj_param + ">"
        self.as_cj_param = "Option<" + self.option_arg_info.as_cj_param + ">"

    def from_cj(self, target: CJSourceWriter, name: str, cj_type: str) -> str:
        target.writelns(
            f"        let temp1_{name} = LibC.malloc<{self.option_arg_info.as_cj_param}>()",
            f"        match ({name}) {{",
            f"            case Some(opt) => temp1_{name}.write(opt)",
            f"            case None => temp1_{name}",
            f"        }}",
            f"        let temp2_{name} = TOptional(CPointer<Unit>(temp1_{name}))",
        )
        return f"temp2_{name}"

    def into_cj(
        self,
        target: CJSourceWriter,
    ):
        target.writelns(
            f"        let cjRes: ?{self.option_arg_info.as_cj_param}",
            f"        if (cRes.m_data.isNull()) {{",
            f"            cjRes = None",
            f"        }} else {{",
            f"            let data = CPointer<{self.option_arg_info.as_cj_param}>(cRes.m_data)",
            f"            cjRes = Option<{self.option_arg_info.as_cj_param}>.Some(data.read())",
            f"        }}",
        )

    def free(
        self,
        target: CJSourceWriter,
        name: str,
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

    def from_cj(self, target: CJSourceWriter, name: str, cj_type: str) -> str:
        return name

    def into_cj(
        self,
        target: CJSourceWriter,
    ):
        target.writeln(f"        let cjRes = cRes")

    def free(
        self,
        target: CJSourceWriter,
        name: str,
        cj_type: str,
    ):
        pass


class TypeCJInfoDispatcher(TypeVisitor[TypeCJInfo]):
    def __init__(self, am: AnalysisManager):
        self.am = am

    @override
    def visit_enum_type(self, t: EnumType) -> TypeCJInfo:
        return EnumTypeCJInfo(self.am, t)

    @override
    def visit_union_type(self, t: UnionType) -> TypeCJInfo:
        return UnionTypeCJInfo(self.am, t)

    @override
    def visit_struct_type(self, t: StructType) -> TypeCJInfo:
        return StructTypeCJInfo(self.am, t)

    @override
    def visit_iface_type(self, t: IfaceType) -> TypeCJInfo:
        return IfaceTypeCJInfo(self.am, t)

    @override
    def visit_scalar_type(self, t: ScalarType) -> TypeCJInfo:
        return ScalarTypeCJInfo(self.am, t)

    @override
    def visit_string_type(self, t: StringType) -> TypeCJInfo:
        return StringTypeCJInfo(self.am, t)

    @override
    def visit_array_type(self, t: ArrayType) -> TypeCJInfo:
        return ArrayTypeCJInfo(self.am, t)

    @override
    def visit_optional_type(self, t: OptionalType) -> TypeCJInfo:
        return OptionalTypeCJInfo(self.am, t)

    @override
    def visit_opaque_type(self, t: OpaqueType) -> TypeCJInfo:
        raise NotImplementedError("OpaqueType is not supported in CJ yet.")

    @override
    def visit_map_type(self, t: MapType) -> TypeCJInfo:
        raise NotImplementedError("MapType is not supported in CJ yet.")

    @override
    def visit_set_type(self, t: SetType) -> TypeCJInfo:
        raise NotImplementedError("SetType is not supported in CJ yet.")

    @override
    def visit_callback_type(self, t: CallbackType):
        return CallbackTypeCJInfo(self.am, t)
