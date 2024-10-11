"""Defines the high-level types inside a compiler."""

from abc import ABC, abstractmethod
from dataclasses import dataclass
from enum import IntFlag, auto
from pathlib import Path
from typing import Any, Optional, override

from taihe.exceptions import (
    EnumValueCollisionError,
    PackageAliasConflictError,
    PackageNameConflictError,
    PackageNotExistError,
    PackageNotImportedError,
    QualifierError,
    SymbolConflictError,
    SymbolConflictWithNamespaceError,
    TypeAliasConflictError,
    TypeNotExistError,
    TypeNotImportedError,
)
from taihe.parse import ast

############################
# Infrastructure for Types #
############################


NamespaceTree = dict[str, "NamespaceTree"]


class TypeQualifier(IntFlag):
    NONE = 0
    MUT = auto()


class Type:
    pass


class UserType(Type):
    pkg_name: tuple[str, ...]
    decl_name: str
    target: Optional["TypeDeclBase"]

    def __init__(self, pkg_name, decl_name):
        self.pkg_name = pkg_name
        self.decl_name = decl_name
        self.target = None


##################
# Built-in Types #
##################


@dataclass(frozen=True)
class PrimitiveType(Type):
    name: str

    @staticmethod
    def lookup(name: str) -> Optional["PrimitiveType"]:
        return _TYPE_MAPS.get(name)


@dataclass(frozen=True)
class IntegerType(PrimitiveType):
    width: int
    is_signed: bool


I8 = IntegerType("i8", 8, is_signed=True)
I16 = IntegerType("i16", 16, is_signed=True)
I32 = IntegerType("i32", 32, is_signed=True)
I64 = IntegerType("i64", 64, is_signed=True)

U8 = IntegerType("u8", 8, is_signed=False)
U16 = IntegerType("u16", 16, is_signed=False)
U32 = IntegerType("u32", 32, is_signed=False)
U64 = IntegerType("u64", 64, is_signed=False)


@dataclass(frozen=True)
class FloatType(PrimitiveType):
    width: int


F16 = FloatType("f16", 16)
F32 = FloatType("f32", 32)


@dataclass(frozen=True)
class BoolType(PrimitiveType):
    pass


BOOL = BoolType("bool")


@dataclass(frozen=True)
class StringType(PrimitiveType):
    pass


STRING = StringType("String")


_TYPE_MAPS: dict[str, PrimitiveType] = {ty.name: ty for ty in [I8, I16, I32, I64, U8, U16, U32, U64, F16, F32, BOOL, STRING]}


################
# Declarations #
################


class PackageRef:
    name: tuple[str, ...]
    target: Optional["PackageBase"]

    def __init__(self, name: tuple[str, ...]):
        self.name = name
        self.target = None


class PackageImport:
    name: tuple[str, ...]
    pkg_ref: PackageRef

    def __init__(self, name: tuple[str, ...], pkg_ref: PackageRef):
        self.name = name
        self.pkg_ref = pkg_ref


class TypeDeclRef:
    name: str
    target: Optional["TypeDeclBase"]

    def __init__(self, pkg_ref: PackageRef, name: str):
        self.name = name
        self.target = None


class TypeImport:
    name: str
    pkg_ref: PackageRef
    type_decl_ref: TypeDeclRef

    def __init__(self, name: str, pkg_ref: PackageRef, type_decl_ref: TypeDeclRef):
        self.name = name
        self.pkg_ref = pkg_ref
        self.type_decl_ref = type_decl_ref


class SpecFieldDecl:
    parent: "Package"
    name: str


class ParamDecl:
    parent: "FuncDecl"
    name: str

    qual: TypeQualifier
    ty: Type

    def __init__(self, parent: "FuncDecl", name: str, qual: TypeQualifier, ty: Type):
        self.parent = parent
        self.name = name
        self.qual = qual
        self.ty = ty
        if qual.value == TypeQualifier.MUT:
            raise  # QualifierError


class FuncDecl(SpecFieldDecl):
    params: dict[str, ParamDecl]
    return_types: list[Type]

    def __init__(self, parent: "Package", name: str):
        self.parent = parent
        self.name = name
        self.params = {}
        self.return_types = []

    def add_param(self, name: str, qual: TypeQualifier, ty: Type) -> ParamDecl:
        param = ParamDecl(self, name, qual, ty)
        if self.params.get(name) is not None:
            raise  # SymbolConflictError
        self.params[name] = param
        return param

    def add_return_type(self, ty: Type):
        self.return_types.append(ty)


class TypeDeclBase:
    pass


class UnknownTypeDecl(TypeDeclBase):
    pass


class TypeDecl(TypeDeclBase, SpecFieldDecl):
    pass


class StructFieldDecl:
    parent: "StructDecl"
    name: str

    ty: Type

    def __init__(self, parent: "StructDecl", name: str, ty: Type):
        self.parent = parent
        self.name = name
        self.ty = ty


class StructDecl(TypeDecl):
    fields: dict[str, StructFieldDecl]

    def __init__(self, parent: "Package", name: str):
        self.parent = parent
        self.name = name
        self.fields = {}

    def add_field(self, name: str, ty: Type) -> StructFieldDecl:
        field = StructFieldDecl(self, name, ty)
        if self.fields.get(name) is not None:
            raise  # SymbolConflictError
        self.fields[name] = field
        return field


class EnumFieldDecl:
    parent: "EnumDecl"
    name: str

    val: int

    def __init__(self, parent: "EnumDecl", name: str, val: int):
        self.parent = parent
        self.name = name
        self.val = val


class EnumDecl(TypeDecl):
    fields: dict[str, EnumFieldDecl]
    values: dict[int, EnumFieldDecl]
    next_val: int

    def __init__(self, parent: "Package", name: str):
        self.parent = parent
        self.name = name
        self.fields = {}
        self.values = {}
        self.next_val = 0

    def add_field(self, name: str, val: int | None) -> EnumFieldDecl:
        if val is None:
            val = self.next_val
        field = EnumFieldDecl(self, name, val)
        if self.values.get(val) is not None:
            raise  # EnumValueCollisionError
        if self.fields.get(name) is not None:
            raise  # SymbolConflictError
        self.values[val] = field
        self.fields[name] = field
        self.next_val = val + 1
        return field


######################
# The main container #
######################


class PackageBase(ABC):
    @abstractmethod
    def lookup_type(self, name: str, error_manager: list): ...


class UnknownPackage(PackageBase):
    @override
    def lookup_type(self, name: str, error_manager: list):
        return UnknownTypeDecl()


class Package(PackageBase):
    parent: "PackageGroup"
    name: tuple[str, ...]

    namespace_tree: NamespaceTree

    func_decl_table: dict[str, FuncDecl]
    type_decl_table: dict[str, TypeDecl]
    type_import_table: dict[str, TypeImport]
    pkg_import_table: dict[tuple[str, ...], PackageImport]

    def __init__(self, parent: "PackageGroup", name: tuple[str, ...], namespace_tree: NamespaceTree):
        self.parent = parent
        self.name = name
        self.namespace_tree = namespace_tree
        self.func_decl_table = {}
        self.type_decl_table = {}
        self.type_import_table = {}
        self.pkg_import_table = {}

    def import_type(self, name: str, type_import: TypeImport):
        if (self.type_decl_table.get(name) or self.type_import_table.get(name)) is not None:
            raise  # SymbolConflictError
        self.type_import_table[name] = type_import

    def import_pkg(self, pkg_name: tuple[str, ...], pkg_import: PackageImport):
        if self.pkg_import_table.get(pkg_name) is not None:
            raise  # PackageAliasConflictError
        self.pkg_import_table[pkg_name] = pkg_import

    def add_func(self, name: str) -> FuncDecl:
        func_decl = FuncDecl(self, name)
        if self.func_decl_table.get(name) is not None:
            raise  # SymbolConflictError
        if self.namespace_tree.get(name) is not None:
            raise  # SymbolConflictWithNamespaceError
        self.func_decl_table[name] = func_decl
        return func_decl

    def add_struct(self, name: str) -> StructDecl:
        struct_decl = StructDecl(self, name)
        if (self.func_decl_table.get(name) or self.type_decl_table.get(name) or self.type_import_table.get(name)) is not None:
            raise  # SymbolConflictError
        if self.namespace_tree.get(name) is not None:
            raise  # SymbolConflictWithNamespaceError
        self.type_decl_table[name] = struct_decl
        return struct_decl

    def add_enum(self, name: str) -> EnumDecl:
        enum_decl = EnumDecl(self, name)
        if (self.func_decl_table.get(name) or self.type_decl_table.get(name) or self.type_import_table.get(name)) is not None:
            raise  # SymbolConflictError
        if self.namespace_tree.get(name) is not None:
            raise  # SymbolConflictWithNamespaceError
        self.type_decl_table[name] = enum_decl
        return enum_decl

    @override
    def lookup_type(self, name: str, error_manager: list):
        target = self.type_decl_table.get(name)
        if target is None:
            error_manager.append(TypeNotExistError)
            return UnknownTypeDecl()
        return target

    def resolve_user_type(self, user_type: UserType, error_manager: list):
        if user_type.target is not None:
            return
        if user_type.pkg_name:
            pkg_import = self.pkg_import_table.get(user_type.pkg_name)
            if pkg_import is None:
                raise  # PackageNotImportedError
            assert pkg_import.pkg_ref.target is not None
            user_type.target = pkg_import.pkg_ref.target.lookup_type(user_type.decl_name, error_manager)
            if user_type.target is None:
                raise  # TypeNotExistError
        else:
            user_type.target = self.type_decl_table.get(user_type.decl_name)
            if user_type.target is not None:
                return
            type_import = self.type_import_table.get(user_type.decl_name)
            if type_import is None:
                raise  # TypeNotImportedError
            assert type_import.type_decl_ref.target is not None
            user_type.target = type_import.type_decl_ref.target


class PackageGroup:
    pkg_table: dict[tuple[str, ...], Package]
    namespace_root: NamespaceTree

    def __init__(self):
        self.pkg_table = {}
        self.namespace_root = {}

    def add_package(self, pkg_name: tuple[str, ...]) -> Package:
        namespace = self.namespace_root
        pkg_name_parts: tuple[str, ...] = ()
        for pkg_name_part in pkg_name:
            pkg_name_parts = *pkg_name_parts, pkg_name_part
            pkg = self.pkg_table.get(pkg_name_parts)
            if pkg is not None and (pkg.func_decl_table.get(pkg_name_part) or pkg.type_decl_table.get(pkg_name_part)) is not None:
                raise  # SymbolConflictWithNamespaceError
            namespace = namespace.setdefault(pkg_name_part, {})
        pkg = Package(self, pkg_name, namespace)
        if self.pkg_table.get(pkg_name) is not None:
            raise  # PackageNameConflictError
        self.pkg_table[pkg_name] = pkg
        return pkg

    def lookup_pkg(self, name: tuple[str, ...], error_manager: list):
        target = self.pkg_table.get(name)
        if target is None:
            error_manager.append(PackageNotExistError)
            return UnknownPackage()
        return target

    def check_recursive_inclusion(self):
        pass
