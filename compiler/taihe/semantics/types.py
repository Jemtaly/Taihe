"""Defines the high-level types inside a compiler."""

from abc import ABC, abstractmethod
from dataclasses import dataclass
from enum import IntFlag, auto
from typing import Optional, override

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

############################
# Infrastructure for Types #
############################


ErrorManager = list
NamespaceTree = dict[str, "NamespaceTree"]


class TypeQualifier(IntFlag):
    NONE = 0
    MUT = auto()


class Type:
    pass


class UserType(Type):
    pass


class UnknownType(UserType):
    pass


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


class SpecFieldDecl:
    error_manager: ErrorManager

    parent: "Package"

    name: str


class ParamDecl:
    error_manager: ErrorManager

    parent: "FuncDecl"

    name: str

    qual: TypeQualifier
    ty: Type

    def __init__(self, parent: "FuncDecl", error_manager: ErrorManager, name: str, qual: TypeQualifier, ty: Type):
        self.parent = parent
        self.error_manager = error_manager
        self.name = name
        self.qual = qual
        self.ty = ty
        if qual.value == TypeQualifier.MUT:
            self.error_manager.append(QualifierError)


class FuncDecl(SpecFieldDecl):
    params: list[ParamDecl]
    return_types: list[Type]

    def __init__(self, parent: "Package", error_manager: ErrorManager, name: str):
        self.parent = parent
        self.error_manager = error_manager
        self.name = name
        self.params = []
        self.return_types = []

    def add_param(self, name: str, qual: TypeQualifier, ty: Type) -> ParamDecl:
        param = ParamDecl(self, self.error_manager, name, qual, ty)
        self.params.append(param)
        return param

    def add_return_type(self, ty: Type):
        self.return_types.append(ty)

    def check_param_conflicts(self):
        param_dict: dict[str, list[ParamDecl]] = {}
        for param in self.params:
            param_dict.setdefault(param.name, []).append(param)
        for param_name, params in param_dict.items():
            if len(params) > 1:
                self.error_manager.append(SymbolConflictError)


class TypeDecl(UserType, SpecFieldDecl):
    pass


class StructFieldDecl:
    error_manager: ErrorManager

    parent: "StructDecl"

    name: str

    ty: Type

    def __init__(self, parent: "StructDecl", error_manager: ErrorManager, name: str, ty: Type):
        self.parent = parent
        self.error_manager = error_manager
        self.name = name
        self.ty = ty


class StructDecl(TypeDecl):
    fields: list[StructFieldDecl]

    def __init__(self, parent: "Package", error_manager: ErrorManager, name: str):
        self.parent = parent
        self.error_manager = error_manager
        self.name = name
        self.fields = []

    def add_field(self, name: str, ty: Type) -> StructFieldDecl:
        field = StructFieldDecl(self, self.error_manager, name, ty)
        self.fields.append(field)
        return field

    def check_field_conflicts(self):
        field_dict: dict[str, list[StructFieldDecl]] = {}
        for field in self.fields:
            field_dict.setdefault(field.name, []).append(field)
        for field_name, fields in field_dict.items():
            if len(fields) > 1:
                self.error_manager.append(SymbolConflictError)


class EnumFieldDecl:
    error_manager: ErrorManager

    parent: "EnumDecl"

    name: str

    val: int

    def __init__(self, parent: "EnumDecl", error_manager: ErrorManager, name: str, val: int):
        self.parent = parent
        self.error_manager = error_manager
        self.name = name
        self.val = val


class EnumDecl(TypeDecl):
    fields: list[EnumFieldDecl]
    next_val: int

    def __init__(self, parent: "Package", error_manager: ErrorManager, name: str):
        self.parent = parent
        self.error_manager = error_manager
        self.name = name
        self.fields = []
        self.next_val = 0

    def add_field(self, name: str, val: int | None) -> EnumFieldDecl:
        if val is not None:
            self.next_val = val
        field = EnumFieldDecl(self, self.error_manager, name, self.next_val)
        self.fields.append(field)
        self.next_val += 1
        return field

    def check_field_conflicts(self):
        field_dict: dict[str, list[EnumFieldDecl]] = {}
        for field in self.fields:
            field_dict.setdefault(field.name, []).append(field)
        for field_name, fields in field_dict.items():
            if len(fields) > 1:
                self.error_manager.append(SymbolConflictError)

    def check_value_conflicts(self):
        value_dict: dict[int, list[EnumFieldDecl]] = {}
        for field in self.fields:
            value_dict.setdefault(field.val, []).append(field)
        for field_name, fields in value_dict.items():
            if len(fields) > 1:
                self.error_manager.append(EnumValueCollisionError)


######################
# The main container #
######################


class PackageBase(ABC):
    parent: "PackageGroup"

    error_manager: ErrorManager

    name: tuple[str, ...]

    @abstractmethod
    def lookup(self, type_name: str) -> UserType: ...


class Package(PackageBase):
    type_table: dict[str, list[TypeDecl]]

    imported_type_dict: dict[str, list[tuple[UserType, list]]]
    imported_pkg_dict: dict[tuple[str, ...], list[tuple[PackageBase, list]]]

    decls: list[SpecFieldDecl]  # All declarations, including type declarations and function declarations.
    namespace_tree: NamespaceTree

    def __init__(self, parent: "PackageGroup", error_manager: ErrorManager, name: tuple[str, ...], namespace_tree: NamespaceTree):
        self.parent = parent
        self.error_manager = error_manager
        self.name = name
        self.imported_type_dict = {}
        self.imported_pkg_dict = {}
        self.type_table = {}
        self.decls = []
        self.namespace_tree = namespace_tree

    def import_types(self, pkg_name: tuple[str, ...], orig_type_names: list[str], type_names: list[str]):
        orig_pkg = self.parent.lookup(pkg_name)
        for type_name, orig_type_name in zip(type_names, orig_type_names, strict=True):
            orig_type_decl = orig_pkg.lookup(orig_type_name)
            self.imported_type_dict.setdefault(type_name, []).append((orig_type_decl, []))

    def import_pkg(self, orig_pkg_name: tuple[str, ...], pkg_name: tuple[str, ...]):
        orig_pkg = self.parent.lookup(orig_pkg_name)
        self.imported_pkg_dict.setdefault(pkg_name, []).append((orig_pkg, []))

    def decl_func(self, name: str) -> FuncDecl:
        func_decl = FuncDecl(self, self.error_manager, name)
        self.decls.append(func_decl)
        return func_decl

    def decl_struct(self, name: str) -> StructDecl:
        struct_decl = StructDecl(self, self.error_manager, name)
        self.decls.append(struct_decl)
        self.type_table.setdefault(name, []).append(struct_decl)
        return struct_decl

    def decl_enum(self, name: str) -> EnumDecl:
        enum_decl = EnumDecl(self, self.error_manager, name)
        self.decls.append(enum_decl)
        self.type_table.setdefault(name, []).append(enum_decl)
        return enum_decl

    def check_decl_conflicts(self):
        decl_dict: dict[str, list[SpecFieldDecl]] = {}
        for decl in self.decls:
            decl_dict.setdefault(decl.name, []).append(decl)
        for decl_name, decls in decl_dict.items():
            if len(decls) > 1:
                self.error_manager.append(SymbolConflictError)
            if self.namespace_tree.get(decl_name) is not None:
                self.error_manager.append(SymbolConflictWithNamespaceError)

    def import_local_types(self):
        for type_name, type_decls in self.type_table.items():
            user_type = UnknownType() if len(type_decls) != 1 else type_decls[0]
            self.imported_type_dict.setdefault(type_name, []).append((user_type, []))

    def check_import_conflicts(self):
        for imported_type_name, imported_types in self.imported_type_dict.items():
            if len(imported_types) > 1:
                self.error_manager.append(TypeAliasConflictError)
        for imported_pkg_name, imported_pkgs in self.imported_pkg_dict.items():
            if len(imported_pkgs) > 1:
                self.error_manager.append(PackageAliasConflictError)

    @override
    def lookup(self, type_name: str) -> UserType:
        if (target_type_decls := self.type_table.get(type_name)) is None:
            self.error_manager.append(TypeNotExistError)
            return UnknownType()
        if len(target_type_decls) != 1:
            return UnknownType()
        return target_type_decls[0]

    def resolve(self, pkg_name: tuple[str, ...], type_name: str) -> UserType:
        if pkg_name:
            if (orig_pkg_names := self.imported_pkg_dict.get(pkg_name)) is None:
                self.error_manager.append(PackageNotImportedError)
                return UnknownType()
            if len(orig_pkg_names) != 1:
                return UnknownType()
            orig_pkg, _ = orig_pkg_names[0]
            orig_user_type = orig_pkg.lookup(type_name)
        else:
            if (orig_type_names := self.imported_type_dict.get(type_name)) is None:
                self.error_manager.append(TypeNotImportedError)
                return UnknownType()
            if len(orig_type_names) != 1:
                return UnknownType()
            orig_user_type, _ = orig_type_names[0]
        return orig_user_type


class DuplicatePackage(PackageBase):
    pkgs: list[Package]

    def __init__(self, parent: "PackageGroup", error_manager: ErrorManager, name: tuple[str, ...], pkgs: list[Package]):
        self.parent = parent
        self.error_manager = error_manager
        self.name = name
        self.pkgs = pkgs

    @override
    def lookup(self, type_name: str) -> UserType:
        target_type_decls: list[TypeDecl] = []
        for pkg in self.pkgs:
            target_type_decls += pkg.type_table.get(type_name, [])
        if len(target_type_decls) == 0:
            self.error_manager.append(TypeNotExistError)
            return UnknownType()
        if len(target_type_decls) != 1:
            return UnknownType()
        return target_type_decls[0]


class UnknownPackage(PackageBase):
    def __init__(self, parent: "PackageGroup", error_manager: ErrorManager, name: tuple[str, ...]):
        self.parent = parent
        self.error_manager = error_manager
        self.name = name

    @override
    def lookup(self, type_name: str) -> UserType:
        return UnknownType()


class PackageGroup:
    error_manager: ErrorManager

    pkg_table: dict[tuple[str, ...], list[Package]]
    namespace_root: NamespaceTree

    def __init__(self, error_manager: ErrorManager):
        self.pkg_table = {}
        self.error_manager = error_manager
        self.namespace_root = {}

    def lookup(self, name: tuple[str, ...]) -> PackageBase:
        if (target_pkgs := self.pkg_table.get(name)) is None:
            self.error_manager.append(PackageNotExistError)
            return UnknownPackage(self, self.error_manager, name)
        if len(target_pkgs) != 1:
            return DuplicatePackage(self, self.error_manager, name, target_pkgs)
        return target_pkgs[0]

    def add_package(self, pkg_name: tuple[str, ...]) -> Package:
        namespace = self.namespace_root
        for pkg_name_part in pkg_name:
            namespace = namespace.setdefault(pkg_name_part, {})
        package = Package(self, self.error_manager, pkg_name, namespace)
        self.pkg_table.setdefault(pkg_name, []).append(package)
        return package

    def check_package_conflicts(self):
        for pkg_name, pkgs in self.pkg_table.items():
            if len(pkgs) != 1:
                self.error_manager.append(PackageNameConflictError)

    def check_recursive_inclusion(self):
        pass
