from io import StringIO
from os import PathLike
from pathlib import Path
from typing import Any

from typing_extensions import override

from taihe.codegen.mangle import DeclKind, encode
from taihe.semantics.declarations import (
    EnumDecl,
    FuncDecl,
    IfaceDecl,
    Package,
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
    ScalarType,
    SpecialType,
    TypeAlike,
)
from taihe.semantics.visitor import DeclVisitor, TypeVisitor
from taihe.utils.analyses import AbstractAnalysis, AnalysisManager
from taihe.utils.outputs import OutputBase, OutputManager


class COutputBuffer(OutputBase):
    """Represents a C or C++ target file."""

    def __init__(self, filename: str):
        super().__init__(filename)
        self.headers: set[str] = set()
        self.code = StringIO()

    @override
    def output_to(self, dst_path: PathLike):
        with open(Path(dst_path) / self.filename, "w", encoding="utf-8") as dst:
            if self.filename.endswith((".h", ".hpp")):
                dst.write(f"#pragma once\n")
            for header in self.headers:
                dst.write(f"#include <{header}>\n")
            dst.write(self.code.getvalue())

    @override
    def show(self):
        print(f"// {self.filename}")
        if self.filename.endswith((".h", ".hpp")):
            print(f"#pragma once")
        for header in self.headers:
            print(f"#include <{header}>")
        print(self.code.getvalue())

    def write(self, code: str):
        self.code.write(code)

    def include(self, *headers: "str | COutputBuffer | None"):
        for header in headers:
            if isinstance(header, str):
                self.headers.add(header)
            elif isinstance(header, COutputBuffer):
                self.headers.add(header.filename)


class ABIFuncDeclInfo(AbstractAnalysis):
    def __init__(self, am: AnalysisManager, d: FuncDecl) -> None:
        pkg = d.parent
        assert pkg
        segments = [*pkg.name.split("."), d.name]
        self.name = encode(segments, DeclKind.FUNCTION)


class ABIStructDeclInfo(AbstractAnalysis):
    def __init__(self, am: AnalysisManager, d: StructDecl) -> None:
        pkg = d.parent
        assert pkg
        segments = [*pkg.name.split("."), d.name]
        self.header = pkg.name + "." + d.name + ".h"
        self.name = encode(segments, DeclKind.STRUCT)


class ABIEnumDeclInfo(AbstractAnalysis):
    def __init__(self, am: AnalysisManager, d: EnumDecl) -> None:
        pkg = d.parent
        assert pkg
        segments = [*pkg.name.split("."), d.name]
        self.header = pkg.name + "." + d.name + ".h"
        self.name = encode(segments, DeclKind.ENUM)


class ABIIfaceDeclInfo(AbstractAnalysis):
    def __init__(self, am: AnalysisManager, d: IfaceDecl) -> None:
        pkg = d.parent
        assert pkg
        segments = [*pkg.name.split("."), d.name]
        self.header = pkg.name + "." + d.name + ".h"
        self.name = encode(segments, DeclKind.INTERFACE)
        self.f_table = "FTable_" + self.name
        self.v_table = "VTable_" + self.name


class ABIParamTypeRefDeclInfo(AbstractAnalysis, TypeVisitor):
    def __init__(self, am: AnalysisManager, t: TypeAlike) -> None:
        self.handle_type(t)


class ABINormalTypeRefDeclInfo(AbstractAnalysis, TypeVisitor):
    def __init__(self, am: AnalysisManager, t: TypeAlike) -> None:
        self.am = am
        self.header = None
        self.name = None
        self.handle_type(t)

    @override
    def visit_enum_decl(self, d: EnumDecl) -> Any:
        abi_enum_info = ABIEnumDeclInfo(self.am, d)
        self.header = abi_enum_info.header
        self.name = "enum " + abi_enum_info.name

    @override
    def visit_struct_decl(self, d: StructDecl) -> Any:
        abi_struct_info = ABIStructDeclInfo(self.am, d)
        self.header = abi_struct_info.header
        self.name = "struct " + abi_struct_info.name

    @override
    def visit_iface_decl(self, d: IfaceDecl) -> Any:
        abi_iface_info = ABIIfaceDeclInfo(self.am, d)
        self.header = abi_iface_info.header
        self.name = "struct " + abi_iface_info.name

    def visit_scalar_type(self, t: ScalarType):
        self.name = {
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
        if self.name is None:
            raise ValueError

    def visit_special_type(self, t: SpecialType) -> Any:
        if t == STRING:
            self.header = "taihe/string.abi.h"
            self.name = "struct TString*"


class ABICodeGenerator(DeclVisitor):
    def __init__(self, tm: OutputManager, am: AnalysisManager):
        self._current_package_group = None
        self._current_package = None
        self._current_abi_target = None
        self.tm = tm
        self.am = am

    @property
    def _pkg(self) -> Package:
        assert self._current_package
        return self._current_package

    @property
    def _pkg_group(self) -> PackageGroup:
        assert self._current_package_group
        return self._current_package_group

    @override
    def visit_package_group(self, g: PackageGroup):
        self._current_package_group = g
        super().visit_package_group(g)
        self._current_package_group = None

    @property
    def _abi_target(self) -> COutputBuffer:
        assert self._current_abi_target
        return self._current_abi_target

    @override
    def visit_package(self, p: Package):
        pkg_name = p.name
        self._current_abi_target = COutputBuffer(pkg_name)
        self.tm.add(self._current_abi_target)

        self._current_abi_target.include("taihe/common.h")

        self._current_package = p
        super().visit_package(p)
        self._current_package = None

        self._current_abi_target = None

    @override
    def visit_enum_decl(self, d: EnumDecl) -> Any:
        enum_abi_info = ABIEnumDeclInfo.get(self.am, d)

        enum_abi_target = COutputBuffer(enum_abi_info.header)
        self.tm.add(enum_abi_target)

        enum_abi_target.write(f"enum {enum_abi_info.name} {{\n")
        for item in d.items:
            enum_abi_target.write(f"  {item.name} = {item.value};\n")
        enum_abi_target.write(f"}};\n")

        self._abi_target.include(enum_abi_target)

    @override
    def visit_struct_decl(self, d: StructDecl) -> Any:
        struct_abi_info = ABIStructDeclInfo.get(self.am, d)

        struct_abi_target = COutputBuffer(struct_abi_info.header)
        self.tm.add(struct_abi_target)

        struct_abi_target.write(f"struct {struct_abi_info.name} {{\n")
        for field in d.fields:
            ty_info = ABINormalTypeRefDeclInfo.get(self.am, field.ty)
            struct_abi_target.include(ty_info.header)
            struct_abi_target.write(f"  {ty_info.name} {field.name};\n")
        struct_abi_target.write(f"}};\n")

        self._abi_target.include(struct_abi_target)

    @override
    def visit_func_decl(self, d: FuncDecl) -> Any:
        pass
