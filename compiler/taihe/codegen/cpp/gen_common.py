# -*- coding: utf-8 -*-
#
# Copyright (c) 2025 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from json import dumps

from taihe.codegen.abi.analyses import (
    EnumAbiInfo,
    IfaceAbiInfo,
    IfaceMethodAbiInfo,
    StructAbiInfo,
    TypeAbiInfo,
    UnionAbiInfo,
)
from taihe.codegen.abi.writer import CHeaderWriter
from taihe.codegen.cpp.analyses import (
    EnumCppInfo,
    IfaceCppInfo,
    IfaceMethodCppInfo,
    PackageCppInfo,
    StructCppInfo,
    TypeCppInfo,
    UnionCppInfo,
    from_abi,
    into_abi,
)
from taihe.semantics.declarations import (
    EnumDecl,
    IfaceDecl,
    IfaceMethodDecl,
    PackageDecl,
    PackageGroup,
    StructDecl,
    UnionDecl,
)
from taihe.semantics.types import (
    NonVoidType,
    ScalarType,
    StringType,
)
from taihe.utils.analyses import AnalysisManager
from taihe.utils.outputs import FileKind, OutputManager


class CppHeadersGenerator:
    def __init__(self, am: AnalysisManager, pg: PackageGroup):
        self.am = am
        self.pg = pg

    def generate(self, om: OutputManager):
        for pkg in self.pg.all_packages:
            for enum in pkg.enums:
                CppEnumDeclGenerator(self.am, enum).gen_enum_decl_file(om)
                CppEnumDefnGenerator(self.am, enum).gen_enum_defn_file(om)
            for struct in pkg.structs:
                CppStructDeclGenerator(self.am, struct).gen_struct_decl_file(om)
                CppStructDefnGenerator(self.am, struct).gen_struct_defn_file(om)
                CppStructImplGenerator(self.am, struct).gen_struct_impl_file(om)
            for union in pkg.unions:
                CppUnionDeclGenerator(self.am, union).gen_union_decl_file(om)
                CppUnionDefnGenerator(self.am, union).gen_union_defn_file(om)
                CppUnionImplGenerator(self.am, union).gen_union_impl_file(om)
            for iface in pkg.interfaces:
                CppIfaceDeclGenerator(self.am, iface).gen_iface_decl_file(om)
                CppIfaceDefnGenerator(self.am, iface).gen_iface_defn_file(om)
                CppIfaceImplGenerator(self.am, iface).gen_iface_impl_file(om)
            CppPackageGenerator(self.am, pkg).gen_package_file(om)


class CppPackageGenerator:
    def __init__(self, am: AnalysisManager, pkg: PackageDecl):
        self.am = am
        self.pkg = pkg

    def gen_package_file(self, om: OutputManager):
        pkg_cpp_info = PackageCppInfo.get(self.am, self.pkg)
        target = CHeaderWriter(
            om,
            f"include/{pkg_cpp_info.header}",
            FileKind.CPP_HEADER,
        )
        with target:
            for enum in self.pkg.enums:
                enum_cpp_info = EnumCppInfo.get(self.am, enum)
                target.add_include(enum_cpp_info.defn_header)
            for struct in self.pkg.structs:
                struct_cpp_info = StructCppInfo.get(self.am, struct)
                target.add_include(struct_cpp_info.impl_header)
            for union in self.pkg.unions:
                union_cpp_info = UnionCppInfo.get(self.am, union)
                target.add_include(union_cpp_info.impl_header)
            for iface in self.pkg.interfaces:
                iface_cpp_info = IfaceCppInfo.get(self.am, iface)
                target.add_include(iface_cpp_info.impl_header)


class CppEnumDeclGenerator:
    def __init__(self, am: AnalysisManager, enum: EnumDecl):
        self.am = am
        self.enum = enum

    def gen_enum_decl_file(self, om: OutputManager):
        enum_abi_info = EnumAbiInfo.get(self.am, self.enum)
        enum_cpp_info = EnumCppInfo.get(self.am, self.enum)
        target = CHeaderWriter(
            om,
            f"include/{enum_cpp_info.decl_header}",
            FileKind.C_HEADER,
        )
        with target:
            target.add_include("taihe/common.hpp")
            with target.indented(
                f"namespace {enum_cpp_info.namespace} {{",
                f"}}",
                indent="",
            ):
                target.writelns(
                    f"struct {enum_cpp_info.name};",
                )
            with target.indented(
                f"namespace taihe {{",
                f"}}",
                indent="",
            ):
                target.writelns(
                    f"template<>",
                )
                with target.indented(
                    f"struct as_abi<{enum_cpp_info.full_name}> {{",
                    f"}};",
                ):
                    target.writelns(
                        f"using type = {enum_abi_info.abi_type};",
                    )
                target.writelns(
                    f"template<>",
                )
                with target.indented(
                    f"struct as_param<{enum_cpp_info.full_name}> {{",
                    f"}};",
                ):
                    target.writelns(
                        f"using type = {enum_cpp_info.full_name};",
                    )


class CppEnumDefnGenerator:
    def __init__(self, am: AnalysisManager, enum: EnumDecl):
        self.am = am
        self.enum = enum

    def gen_enum_defn_file(self, om: OutputManager):
        enum_abi_info = EnumAbiInfo.get(self.am, self.enum)
        enum_cpp_info = EnumCppInfo.get(self.am, self.enum)
        target = CHeaderWriter(
            om,
            f"include/{enum_cpp_info.defn_header}",
            FileKind.CPP_HEADER,
        )
        with target:
            target.add_include(enum_cpp_info.decl_header)
            self.gen_enum_defn(target)
            self.gen_enum_same(target)
            self.gen_enum_hash(target)

    def gen_enum_defn(self, target: CHeaderWriter):
        enum_abi_info = EnumAbiInfo.get(self.am, self.enum)
        enum_cpp_info = EnumCppInfo.get(self.am, self.enum)
        with target.indented(
            f"namespace {enum_cpp_info.namespace} {{",
            f"}}",
            indent="",
        ):
            enum_ty_cpp_info = TypeCppInfo.get(self.am, self.enum.ty)
            target.add_include(*enum_ty_cpp_info.impl_headers)
            with target.indented(
                f"struct {enum_cpp_info.name} {{",
                f"}};",
            ):
                target.writelns(
                    f"public:",
                )
                self.gen_enum_key_type(target)
                self.gen_enum_basic_methods(target)
                self.gen_enum_key_utils(target)
                self.gen_enum_value_utils(target)
                target.writelns(
                    f"private:",
                )
                self.gen_enum_properties(target)

    def gen_enum_key_type(self, target: CHeaderWriter):
        enum_abi_info = EnumAbiInfo.get(self.am, self.enum)
        enum_cpp_info = EnumCppInfo.get(self.am, self.enum)
        with target.indented(
            f"enum class key_t: {enum_abi_info.abi_type} {{",
            f"}};",
        ):
            for item in self.enum.items:
                target.writelns(
                    f"{item.name},",
                )

    def gen_enum_properties(self, target: CHeaderWriter):
        enum_abi_info = EnumAbiInfo.get(self.am, self.enum)
        enum_cpp_info = EnumCppInfo.get(self.am, self.enum)
        target.writelns(
            f"key_t key;",
        )

    def gen_enum_basic_methods(self, target: CHeaderWriter):
        enum_abi_info = EnumAbiInfo.get(self.am, self.enum)
        enum_cpp_info = EnumCppInfo.get(self.am, self.enum)
        # copy constructor
        target.writelns(
            f"{enum_cpp_info.name}({enum_cpp_info.name} const& other) : key(other.key) {{}}",
        )
        # copy assignment
        with target.indented(
            f"{enum_cpp_info.name}& operator=({enum_cpp_info.name} other) {{",
            f"}}",
        ):
            target.writelns(
                f"key = other.key;",
                f"return *this;",
            )

    def gen_enum_key_utils(self, target: CHeaderWriter):
        enum_abi_info = EnumAbiInfo.get(self.am, self.enum)
        enum_cpp_info = EnumCppInfo.get(self.am, self.enum)
        # constructor
        target.writelns(
            f"{enum_cpp_info.name}(key_t key) : key(key) {{}}",
        )
        # key getter
        with target.indented(
            f"key_t get_key() const {{",
            f"}}",
        ):
            target.writelns(
                f"return key;",
            )
        # validity checker
        with target.indented(
            f"bool is_valid() const {{",
            f"}}",
        ):
            target.writelns(
                f"return static_cast<{enum_abi_info.abi_type}>(key) >= 0 && static_cast<{enum_abi_info.abi_type}>(key) < {len(self.enum.items)};",
            )

    def gen_enum_value_utils(self, target: CHeaderWriter):
        enum_abi_info = EnumAbiInfo.get(self.am, self.enum)
        enum_cpp_info = EnumCppInfo.get(self.am, self.enum)
        enum_ty_cpp_info = TypeCppInfo.get(self.am, self.enum.ty)
        match self.enum.ty:
            case StringType():
                as_owner = "char const*"
                as_param = enum_ty_cpp_info.as_param
            case ScalarType():
                as_owner = enum_ty_cpp_info.as_owner
                as_param = enum_ty_cpp_info.as_param
        # table
        with target.indented(
            f"static constexpr {as_owner} table[] = {{",
            f"}};",
        ):
            for item in self.enum.items:
                target.writelns(
                    f"{dumps(item.value)},",
                )
        # value getter
        with target.indented(
            f"{as_owner} get_value() const {{",
            f"}}",
        ):
            target.writelns(
                f"return table[static_cast<{enum_abi_info.abi_type}>(key)];",
            )
        # value converter
        with target.indented(
            f"operator {as_owner}() const {{",
            f"}}",
        ):
            target.writelns(
                f"return table[static_cast<{enum_abi_info.abi_type}>(key)];",
            )
        # creator from value
        with target.indented(
            f"static {enum_cpp_info.as_owner} from_value({as_param} value) {{",
            f"}}",
        ):
            for i, item in enumerate(self.enum.items):
                with target.indented(
                    f"if (value == {dumps(item.value)}) {{",
                    f"}}",
                ):
                    target.writelns(
                        f"return {enum_cpp_info.as_owner}(static_cast<key_t>({i}));",
                    )
            target.writelns(
                f"return {enum_cpp_info.as_owner}(static_cast<key_t>(-1));",
            )

    def gen_enum_same(self, target: CHeaderWriter):
        enum_abi_info = EnumAbiInfo.get(self.am, self.enum)
        enum_cpp_info = EnumCppInfo.get(self.am, self.enum)
        # others
        with target.indented(
            f"namespace {enum_cpp_info.namespace} {{",
            f"}}",
            indent="",
        ):
            with target.indented(
                f"inline bool operator==({enum_cpp_info.as_param} lhs, {enum_cpp_info.as_param} rhs) {{",
                f"}}",
            ):
                target.writelns(
                    f"return lhs.get_key() == rhs.get_key();",
                )

    def gen_enum_hash(self, target: CHeaderWriter):
        enum_abi_info = EnumAbiInfo.get(self.am, self.enum)
        enum_cpp_info = EnumCppInfo.get(self.am, self.enum)
        with target.indented(
            f"template<> struct ::std::hash<{enum_cpp_info.full_name}> {{",
            f"}};",
        ):
            with target.indented(
                f"size_t operator()({enum_cpp_info.as_param} val) const {{",
                f"}}",
            ):
                target.writelns(
                    f"return ::std::hash<{enum_abi_info.abi_type}>()(static_cast<{enum_abi_info.abi_type}>(val.get_key()));",
                )


class CppUnionDeclGenerator:
    def __init__(self, am: AnalysisManager, union: UnionDecl):
        self.am = am
        self.union = union

    def gen_union_decl_file(self, om: OutputManager):
        union_abi_info = UnionAbiInfo.get(self.am, self.union)
        union_cpp_info = UnionCppInfo.get(self.am, self.union)
        target = CHeaderWriter(
            om,
            f"include/{union_cpp_info.decl_header}",
            FileKind.C_HEADER,
        )
        with target:
            target.add_include("taihe/common.hpp")
            target.add_include(union_abi_info.decl_header)
            with target.indented(
                f"namespace {union_cpp_info.namespace} {{",
                f"}}",
                indent="",
            ):
                target.writelns(
                    f"struct {union_cpp_info.name};",
                )
            with target.indented(
                f"namespace taihe {{",
                f"}}",
                indent="",
            ):
                target.writelns(
                    f"template<>",
                )
                with target.indented(
                    f"struct as_abi<{union_cpp_info.as_owner}> {{",
                    f"}};",
                ):
                    target.writelns(
                        f"using type = {union_abi_info.as_owner};",
                    )
                target.writelns(
                    f"template<>",
                )
                with target.indented(
                    f"struct as_abi<{union_cpp_info.as_param}> {{",
                    f"}};",
                ):
                    target.writelns(
                        f"using type = {union_abi_info.as_param};",
                    )
                target.writelns(
                    f"template<>",
                )
                with target.indented(
                    f"struct as_param<{union_cpp_info.as_owner}> {{",
                    f"}};",
                ):
                    target.writelns(
                        f"using type = {union_cpp_info.as_param};",
                    )


class CppUnionDefnGenerator:
    def __init__(self, am: AnalysisManager, union: UnionDecl):
        self.am = am
        self.union = union

    def gen_union_defn_file(self, om: OutputManager):
        union_abi_info = UnionAbiInfo.get(self.am, self.union)
        union_cpp_info = UnionCppInfo.get(self.am, self.union)
        target = CHeaderWriter(
            om,
            f"include/{union_cpp_info.defn_header}",
            FileKind.CPP_HEADER,
        )
        with target:
            target.add_include(union_cpp_info.decl_header)
            target.add_include(union_abi_info.defn_header)
            for field in self.union.fields:
                field_ty_cpp_info = TypeCppInfo.get(self.am, field.ty)
                target.add_include(*field_ty_cpp_info.defn_headers)
            self.gen_union_defn(target)
            self.gen_union_same(target)
            self.gen_union_hash(target)

    def gen_union_defn(self, target: CHeaderWriter):
        union_abi_info = UnionAbiInfo.get(self.am, self.union)
        union_cpp_info = UnionCppInfo.get(self.am, self.union)
        with target.indented(
            f"namespace {union_cpp_info.namespace} {{",
            f"}}",
            indent="",
        ):
            with target.indented(
                f"struct {union_cpp_info.name} {{",
                f"}};",
            ):
                target.writelns(
                    f"public:",
                )
                self.gen_union_tag_type(target)
                self.gen_union_storage_type(target)
                self.gen_union_basic_methods(target)
                self.gen_union_utils(target)
                self.gen_union_named_utils(target)
                target.writelns(
                    f"private:",
                )
                self.gen_union_properties(target)

    def gen_union_tag_type(self, target: CHeaderWriter):
        union_abi_info = UnionAbiInfo.get(self.am, self.union)
        union_cpp_info = UnionCppInfo.get(self.am, self.union)
        with target.indented(
            f"enum class tag_t : {union_abi_info.tag_type} {{",
            f"}};",
        ):
            for field in self.union.fields:
                target.writelns(
                    f"{field.name},",
                )

    def gen_union_storage_type(self, target: CHeaderWriter):
        union_abi_info = UnionAbiInfo.get(self.am, self.union)
        union_cpp_info = UnionCppInfo.get(self.am, self.union)
        with target.indented(
            f"union storage_t {{",
            f"}};",
        ):
            target.writelns(
                f"storage_t() {{}}",
                f"~storage_t() {{}}",
            )
            for field in self.union.fields:
                field_ty_cpp_info = TypeCppInfo.get(self.am, field.ty)
                target.writelns(
                    f"{field_ty_cpp_info.as_owner} {field.name};",
                )

    def gen_union_properties(self, target: CHeaderWriter):
        union_abi_info = UnionAbiInfo.get(self.am, self.union)
        union_cpp_info = UnionCppInfo.get(self.am, self.union)
        target.writelns(
            f"tag_t m_tag;",
            f"storage_t m_data;",
        )

    def gen_union_basic_methods(self, target: CHeaderWriter):
        union_abi_info = UnionAbiInfo.get(self.am, self.union)
        union_cpp_info = UnionCppInfo.get(self.am, self.union)
        # copy constructor
        with target.indented(
            f"{union_cpp_info.name}({union_cpp_info.name} const& other) : m_tag(other.m_tag) {{",
            f"}}",
        ):
            with target.indented(
                f"switch (m_tag) {{",
                f"}}",
                indent="",
            ):
                for field in self.union.fields:
                    with target.indented(
                        f"case tag_t::{field.name}: {{",
                        f"}}",
                    ):
                        target.writelns(
                            f"new (&m_data.{field.name}) decltype(m_data.{field.name})(other.m_data.{field.name});",
                            f"break;",
                        )
        # move constructor
        with target.indented(
            f"{union_cpp_info.name}({union_cpp_info.name}&& other) : m_tag(other.m_tag) {{",
            f"}}",
        ):
            with target.indented(
                f"switch (m_tag) {{",
                f"}}",
                indent="",
            ):
                for field in self.union.fields:
                    with target.indented(
                        f"case tag_t::{field.name}: {{",
                        f"}}",
                    ):
                        target.writelns(
                            f"new (&m_data.{field.name}) decltype(m_data.{field.name})(::std::move(other.m_data.{field.name}));",
                            f"break;",
                        )
        # destructor
        with target.indented(
            f"~{union_cpp_info.name}() {{",
            f"}}",
        ):
            with target.indented(
                f"switch (m_tag) {{",
                f"}}",
                indent="",
            ):
                for field in self.union.fields:
                    with target.indented(
                        f"case tag_t::{field.name}: {{",
                        f"}}",
                    ):
                        target.writelns(
                            f"::std::destroy_at(&m_data.{field.name});",
                            f"break;",
                        )
        # copy assignment
        with target.indented(
            f"{union_cpp_info.name}& operator=({union_cpp_info.name} const& other) {{",
            f"}}",
        ):
            with target.indented(
                f"if (this != &other) {{",
                f"}}",
            ):
                target.writelns(
                    f"::std::destroy_at(this);",
                    f"new (this) {union_cpp_info.name}(other);",
                )
            target.writelns(
                f"return *this;",
            )
        # move assignment
        with target.indented(
            f"{union_cpp_info.name}& operator=({union_cpp_info.name}&& other) {{",
            f"}}",
        ):
            with target.indented(
                f"if (this != &other) {{",
                f"}}",
            ):
                target.writelns(
                    f"::std::destroy_at(this);",
                    f"new (this) {union_cpp_info.name}(::std::move(other));",
                )
            target.writelns(
                f"return *this;",
            )

    def gen_union_utils(self, target: CHeaderWriter):
        union_abi_info = UnionAbiInfo.get(self.am, self.union)
        union_cpp_info = UnionCppInfo.get(self.am, self.union)
        # in place constructor
        target.writelns(
            f"template<tag_t tag, typename... Args>",
        )
        with target.indented(
            f"{union_cpp_info.name}(::taihe::static_tag_t<tag>, Args&&... args) : m_tag(tag) {{",
            f"}}",
        ):
            for field in self.union.fields:
                with target.indented(
                    f"if constexpr (tag == tag_t::{field.name}) {{",
                    f"}}",
                ):
                    target.writelns(
                        f"new (&m_data.{field.name}) decltype(m_data.{field.name})(::std::forward<Args>(args)...);",
                    )
        # creator
        target.writelns(
            f"template<tag_t tag, typename... Args>",
        )
        with target.indented(
            f"static {union_cpp_info.name} make(Args&&... args) {{",
            f"}}",
        ):
            target.writelns(
                f"return {union_cpp_info.name}(::taihe::static_tag<tag>, ::std::forward<Args>(args)...);",
            )
        # emplacement
        target.writelns(
            f"template<tag_t tag, typename... Args>",
        )
        with target.indented(
            f"auto& emplace(Args&&... args) & {{",
            f"}}",
        ):
            target.writelns(
                f"::std::destroy_at(this);",
                f"new (this) {union_cpp_info.name}(::taihe::static_tag<tag>, ::std::forward<Args>(args)...);",
                f"return get_ref<tag>();",
            )
        # tag getter
        with target.indented(
            f"tag_t get_tag() const {{",
            f"}}",
        ):
            target.writelns(
                f"return m_tag;",
            )
        # tag checker
        target.writelns(
            f"template<tag_t tag>",
        )
        with target.indented(
            f"bool holds() const {{",
            f"}}",
        ):
            target.writelns(
                f"return m_tag == tag;",
            )
        for constness in ["", " const"]:
            # pointer getter
            target.writelns(
                f"template<tag_t tag>",
            )
            with target.indented(
                f"auto{constness}* get_ptr(){constness} {{",
                f"}}",
            ):
                for field in self.union.fields:
                    with target.indented(
                        f"if constexpr (tag == tag_t::{field.name}) {{",
                        f"}}",
                    ):
                        target.writelns(
                            f"return m_tag == tag_t::{field.name} ? &m_data.{field.name} : nullptr;",
                        )
            # lvalue reference getter
            target.writelns(
                f"template<tag_t tag>",
            )
            with target.indented(
                f"auto{constness}& get_ref(){constness}& {{",
                f"}}",
            ):
                for field in self.union.fields:
                    with target.indented(
                        f"if constexpr (tag == tag_t::{field.name}) {{",
                        f"}}",
                    ):
                        target.writelns(
                            f"return m_data.{field.name};",
                        )
            # rvalue reference getter
            target.writelns(
                f"template<tag_t tag>",
            )
            with target.indented(
                f"auto{constness}&& get_ref(){constness}&& {{",
                f"}}",
            ):
                for field in self.union.fields:
                    with target.indented(
                        f"if constexpr (tag == tag_t::{field.name}) {{",
                        f"}}",
                    ):
                        target.writelns(
                            f"return std::move(m_data).{field.name};",
                        )
            # lvalue reference visitor
            target.writelns(
                f"template<typename ReturnType, typename Visitor>",
            )
            with target.indented(
                f"ReturnType visit(Visitor&& visitor){constness}& {{",
                f"}}",
            ):
                with target.indented(
                    f"switch (m_tag) {{",
                    f"}}",
                    indent="",
                ):
                    for field in self.union.fields:
                        with target.indented(
                            f"case tag_t::{field.name}: {{",
                            f"}}",
                        ):
                            target.writelns(
                                f"return visitor(::taihe::static_tag<tag_t::{field.name}>, m_data.{field.name});",
                            )
            # rvalue reference visitor
            target.writelns(
                f"template<typename ReturnType, typename Visitor>",
            )
            with target.indented(
                f"ReturnType visit(Visitor&& visitor){constness}&& {{",
                f"}}",
            ):
                with target.indented(
                    f"switch (m_tag) {{",
                    f"}}",
                    indent="",
                ):
                    for field in self.union.fields:
                        with target.indented(
                            f"case tag_t::{field.name}: {{",
                            f"}}",
                        ):
                            target.writelns(
                                f"return visitor(::taihe::static_tag<tag_t::{field.name}>, std::move(m_data).{field.name});",
                            )

    def gen_union_named_utils(self, target: CHeaderWriter):
        union_abi_info = UnionAbiInfo.get(self.am, self.union)
        union_cpp_info = UnionCppInfo.get(self.am, self.union)
        # creator
        for field in self.union.fields:
            target.writelns(
                f"template<typename... Args>",
            )
            with target.indented(
                f"static {union_cpp_info.name} make_{field.name}(Args&&... args) {{",
                f"}}",
            ):
                target.writelns(
                    f"return make<tag_t::{field.name}>(::std::forward<Args>(args)...);",
                )
        # emplacement
        for field in self.union.fields:
            target.writelns(
                f"template<typename... Args>",
            )
            with target.indented(
                f"auto& emplace_{field.name}(Args&&... args) {{",
                f"}}",
            ):
                target.writelns(
                    f"return emplace<tag_t::{field.name}>(::std::forward<Args>(args)...);",
                )
        # tag checker
        for field in self.union.fields:
            with target.indented(
                f"bool holds_{field.name}() const {{",
                f"}}",
            ):
                target.writelns(
                    f"return holds<tag_t::{field.name}>();",
                )
        for constness in ["", " const"]:
            # pointer getter
            for field in self.union.fields:
                with target.indented(
                    f"auto{constness}* get_{field.name}_ptr(){constness} {{",
                    f"}}",
                ):
                    target.writelns(
                        f"return m_tag == tag_t::{field.name} ? &m_data.{field.name} : nullptr;",
                    )
            # lvalue reference getter
            for field in self.union.fields:
                with target.indented(
                    f"auto{constness}& get_{field.name}_ref(){constness}& {{",
                    f"}}",
                ):
                    target.writelns(
                        f"return m_data.{field.name};",
                    )
            # rvalue reference getter
            for field in self.union.fields:
                with target.indented(
                    f"auto{constness}&& get_{field.name}_ref(){constness}&& {{",
                    f"}}",
                ):
                    target.writelns(
                        f"return std::move(m_data).{field.name};",
                    )
            # lvalue reference matcher
            target.writelns(
                f"template<typename ReturnType, typename Matcher>",
            )
            with target.indented(
                f"ReturnType match(Matcher&& matcher){constness}& {{",
                f"}}",
            ):
                with target.indented(
                    f"switch (m_tag) {{",
                    f"}}",
                    indent="",
                ):
                    for field in self.union.fields:
                        with target.indented(
                            f"case tag_t::{field.name}: {{",
                            f"}}",
                        ):
                            target.writelns(
                                f"return matcher.case_{field.name}(m_data.{field.name});",
                            )
            # rvalue reference matcher
            target.writelns(
                f"template<typename ReturnType, typename Matcher>",
            )
            with target.indented(
                f"ReturnType match(Matcher&& matcher){constness}&& {{",
                f"}}",
            ):
                with target.indented(
                    f"switch (m_tag) {{",
                    f"}}",
                    indent="",
                ):
                    for field in self.union.fields:
                        with target.indented(
                            f"case tag_t::{field.name}: {{",
                            f"}}",
                        ):
                            target.writelns(
                                f"return matcher.case_{field.name}(std::move(m_data).{field.name});",
                            )

    def gen_union_same(self, target: CHeaderWriter):
        union_abi_info = UnionAbiInfo.get(self.am, self.union)
        union_cpp_info = UnionCppInfo.get(self.am, self.union)
        with target.indented(
            f"namespace {union_cpp_info.namespace} {{",
            f"}}",
            indent="",
        ):
            with target.indented(
                f"inline bool operator==({union_cpp_info.as_param} lhs, {union_cpp_info.as_param} rhs) {{",
                f"}}",
            ):
                result = "false"
                for field in self.union.fields:
                    result = f"{result} || (lhs.holds_{field.name}() && rhs.holds_{field.name}() && lhs.get_{field.name}_ref() == rhs.get_{field.name}_ref())"
                target.writelns(
                    f"return {result};",
                )

    def gen_union_hash(self, target: CHeaderWriter):
        union_abi_info = UnionAbiInfo.get(self.am, self.union)
        union_cpp_info = UnionCppInfo.get(self.am, self.union)
        with target.indented(
            f"template<> struct ::std::hash<{union_cpp_info.full_name}> {{",
            f"}};",
        ):
            with target.indented(
                f"size_t operator()({union_cpp_info.as_param} val) const {{",
                f"}}",
            ):
                with target.indented(
                    f"switch (val.get_tag()) {{",
                    f"}}",
                    indent="",
                ):
                    for field in self.union.fields:
                        with target.indented(
                            f"case {union_cpp_info.full_name}::tag_t::{field.name}: {{",
                            f"}}",
                        ):
                            target.writelns(
                                f"::std::size_t seed = ::std::hash<{union_abi_info.tag_type}>()(static_cast<{union_abi_info.tag_type}>({union_cpp_info.full_name}::tag_t::{field.name}));",
                                f"return seed ^ (0x9e3779b9 + (seed << 6) + (seed >> 2) + ::std::hash<{TypeCppInfo.get(self.am, field.ty).as_owner}>()(val.get_{field.name}_ref()));",
                            )


class CppUnionImplGenerator:
    def __init__(self, am: AnalysisManager, union: UnionDecl):
        self.am = am
        self.union = union

    def gen_union_impl_file(self, om: OutputManager):
        union_abi_info = UnionAbiInfo.get(self.am, self.union)
        union_cpp_info = UnionCppInfo.get(self.am, self.union)
        target = CHeaderWriter(
            om,
            f"include/{union_cpp_info.impl_header}",
            FileKind.C_HEADER,
        )
        with target:
            target.add_include(union_cpp_info.defn_header)
            target.add_include(union_abi_info.impl_header)
            for field in self.union.fields:
                field_ty_cpp_info = TypeCppInfo.get(self.am, field.ty)
                target.add_include(*field_ty_cpp_info.impl_headers)


class CppStructDeclGenerator:
    def __init__(self, am: AnalysisManager, struct: StructDecl):
        self.am = am
        self.struct = struct

    def gen_struct_decl_file(self, om: OutputManager):
        struct_abi_info = StructAbiInfo.get(self.am, self.struct)
        struct_cpp_info = StructCppInfo.get(self.am, self.struct)
        target = CHeaderWriter(
            om,
            f"include/{struct_cpp_info.decl_header}",
            FileKind.C_HEADER,
        )
        with target:
            target.add_include("taihe/common.hpp")
            target.add_include(struct_abi_info.decl_header)
            with target.indented(
                f"namespace {struct_cpp_info.namespace} {{",
                f"}}",
                indent="",
            ):
                target.writelns(
                    f"struct {struct_cpp_info.name};",
                )
            with target.indented(
                f"namespace taihe {{",
                f"}}",
                indent="",
            ):
                target.writelns(
                    f"template<>",
                )
                with target.indented(
                    f"struct as_abi<{struct_cpp_info.as_owner}> {{",
                    f"}};",
                ):
                    target.writelns(
                        f"using type = {struct_abi_info.as_owner};",
                    )
                target.writelns(
                    f"template<>",
                )
                with target.indented(
                    f"struct as_abi<{struct_cpp_info.as_param}> {{",
                    f"}};",
                ):
                    target.writelns(
                        f"using type = {struct_abi_info.as_param};",
                    )
                target.writelns(
                    f"template<>",
                )
                with target.indented(
                    f"struct as_param<{struct_cpp_info.as_owner}> {{",
                    f"}};",
                ):
                    target.writelns(
                        f"using type = {struct_cpp_info.as_param};",
                    )


class CppStructDefnGenerator:
    def __init__(self, am: AnalysisManager, struct: StructDecl):
        self.am = am
        self.struct = struct

    def gen_struct_defn_file(self, om: OutputManager):
        struct_abi_info = StructAbiInfo.get(self.am, self.struct)
        struct_cpp_info = StructCppInfo.get(self.am, self.struct)
        target = CHeaderWriter(
            om,
            f"include/{struct_cpp_info.defn_header}",
            FileKind.CPP_HEADER,
        )
        with target:
            target.add_include(struct_cpp_info.decl_header)
            target.add_include(struct_abi_info.defn_header)
            for field in self.struct.fields:
                field_ty_cpp_info = TypeCppInfo.get(self.am, field.ty)
                target.add_include(*field_ty_cpp_info.defn_headers)
            self.gen_struct_defn(target)
            self.gen_struct_same(target)
            self.gen_struct_hash(target)

    def gen_struct_defn(self, target: CHeaderWriter):
        struct_abi_info = StructAbiInfo.get(self.am, self.struct)
        struct_cpp_info = StructCppInfo.get(self.am, self.struct)
        with target.indented(
            f"namespace {struct_cpp_info.namespace} {{",
            f"}}",
            indent="",
        ):
            with target.indented(
                f"struct {struct_cpp_info.name} {{",
                f"}};",
            ):
                for field in self.struct.fields:
                    field_ty_cpp_info = TypeCppInfo.get(self.am, field.ty)
                    target.writelns(
                        f"{field_ty_cpp_info.as_owner} {field.name};",
                    )

    def gen_struct_same(self, target: CHeaderWriter):
        struct_abi_info = StructAbiInfo.get(self.am, self.struct)
        struct_cpp_info = StructCppInfo.get(self.am, self.struct)
        with target.indented(
            f"namespace {struct_cpp_info.namespace} {{",
            f"}}",
            indent="",
        ):
            with target.indented(
                f"inline bool operator==({struct_cpp_info.as_param} lhs, {struct_cpp_info.as_param} rhs) {{",
                f"}}",
            ):
                result = "true"
                for field in self.struct.fields:
                    result = f"{result} && lhs.{field.name} == rhs.{field.name}"
                target.writelns(
                    f"return {result};",
                )

    def gen_struct_hash(self, target: CHeaderWriter):
        struct_abi_info = StructAbiInfo.get(self.am, self.struct)
        struct_cpp_info = StructCppInfo.get(self.am, self.struct)
        with target.indented(
            f"template<> struct ::std::hash<{struct_cpp_info.full_name}> {{",
            f"}};",
        ):
            with target.indented(
                f"size_t operator()({struct_cpp_info.as_param} val) const {{",
                f"}}",
            ):
                target.writelns(
                    f"::std::size_t seed = 0;",
                )
                for field in self.struct.fields:
                    target.writelns(
                        f"seed ^= ::std::hash<{TypeCppInfo.get(self.am, field.ty).as_owner}>()(val.{field.name}) + 0x9e3779b9 + (seed << 6) + (seed >> 2);",
                    )
                target.writelns(
                    f"return seed;",
                )


class CppStructImplGenerator:
    def __init__(self, am: AnalysisManager, struct: StructDecl):
        self.am = am
        self.struct = struct

    def gen_struct_impl_file(self, om: OutputManager):
        struct_abi_info = StructAbiInfo.get(self.am, self.struct)
        struct_cpp_info = StructCppInfo.get(self.am, self.struct)
        target = CHeaderWriter(
            om,
            f"include/{struct_cpp_info.impl_header}",
            FileKind.C_HEADER,
        )
        with target:
            target.add_include(struct_cpp_info.defn_header)
            target.add_include(struct_abi_info.impl_header)
            for field in self.struct.fields:
                field_ty_cpp_info = TypeCppInfo.get(self.am, field.ty)
                target.add_include(*field_ty_cpp_info.impl_headers)


class CppIfaceDeclGenerator:
    def __init__(self, am: AnalysisManager, iface: IfaceDecl):
        self.am = am
        self.iface = iface

    def gen_iface_decl_file(self, om: OutputManager):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        target = CHeaderWriter(
            om,
            f"include/{iface_cpp_info.decl_header}",
            FileKind.C_HEADER,
        )
        with target:
            target.add_include("taihe/object.hpp")
            target.add_include(iface_abi_info.decl_header)
            with target.indented(
                f"namespace {iface_cpp_info.weakspace} {{",
                f"}}",
                indent="",
            ):
                target.writelns(
                    f"struct {iface_cpp_info.weak_name};",
                )
            with target.indented(
                f"namespace {iface_cpp_info.namespace} {{",
                f"}}",
                indent="",
            ):
                target.writelns(
                    f"struct {iface_cpp_info.norm_name};",
                )
            with target.indented(
                f"namespace taihe {{",
                f"}}",
                indent="",
            ):
                target.writelns(
                    f"template<>",
                )
                with target.indented(
                    f"struct as_abi<{iface_cpp_info.as_owner}> {{",
                    f"}};",
                ):
                    target.writelns(
                        f"using type = {iface_abi_info.as_owner};",
                    )
                target.writelns(
                    f"template<>",
                )
                with target.indented(
                    f"struct as_abi<{iface_cpp_info.as_param}> {{",
                    f"}};",
                ):
                    target.writelns(
                        f"using type = {iface_abi_info.as_param};",
                    )
                target.writelns(
                    f"template<>",
                )
                with target.indented(
                    f"struct as_param<{iface_cpp_info.as_owner}> {{",
                    f"}};",
                ):
                    target.writelns(
                        f"using type = {iface_cpp_info.as_param};",
                    )


class CppIfaceDefnGenerator:
    def __init__(self, am: AnalysisManager, iface: IfaceDecl):
        self.am = am
        self.iface = iface

    def gen_iface_defn_file(self, om: OutputManager):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        target = CHeaderWriter(
            om,
            f"include/{iface_cpp_info.defn_header}",
            FileKind.CPP_HEADER,
        )
        with target:
            target.add_include(iface_cpp_info.decl_header)
            target.add_include(iface_abi_info.defn_header)
            for ancestor, info in iface_abi_info.ancestor_dict.items():
                if ancestor is self.iface:
                    continue
                ancestor_cpp_info = IfaceCppInfo.get(self.am, ancestor)
                target.add_include(ancestor_cpp_info.defn_header)
            self.gen_iface_view_defn(target)
            self.gen_iface_holder_defn(target)
            self.gen_iface_same(target)
            self.gen_iface_hash(target)

    def gen_iface_view_defn(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        with target.indented(
            f"namespace {iface_cpp_info.weakspace} {{",
            f"}}",
            indent="",
        ):
            with target.indented(
                f"struct {iface_cpp_info.weak_name} {{",
                f"}};",
            ):
                target.writelns(
                    f"static constexpr bool is_holder = false;",
                )
                target.writelns(
                    f"{iface_abi_info.as_owner} m_handle;",
                )
                target.writelns(
                    f"explicit {iface_cpp_info.weak_name}({iface_abi_info.as_param} handle) : m_handle(handle) {{}}",
                )
                self.gen_iface_view_dynamic_cast(target)
                self.gen_iface_view_static_cast(target)
                self.gen_iface_virtual_type_decl(target)
                self.gen_iface_methods_impl_decl(target)
                self.gen_iface_ftbl_decl(target)
                self.gen_iface_vtbl_impl(target)
                self.gen_iface_idmap_impl(target)
                self.gen_iface_infos(target)
                self.gen_iface_utils(target)

    def gen_iface_view_static_cast(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        # static cast to ancestors
        for ancestor, info in iface_abi_info.ancestor_dict.items():
            if ancestor is self.iface:
                continue
            ancestor_cpp_info = IfaceCppInfo.get(self.am, ancestor)
            with target.indented(
                f"operator {ancestor_cpp_info.full_weak_name}() const& {{",
                f"}}",
            ):
                with target.indented(
                    f"return {ancestor_cpp_info.full_weak_name}({{",
                    f"}});",
                ):
                    target.writelns(
                        f"{info.static_cast}(this->m_handle.vtbl_ptr),",
                        f"this->m_handle.data_ptr,",
                    )
            with target.indented(
                f"operator {ancestor_cpp_info.full_norm_name}() const& {{",
                f"}}",
            ):
                with target.indented(
                    f"return {ancestor_cpp_info.full_norm_name}({{",
                    f"}});",
                ):
                    target.writelns(
                        f"{info.static_cast}(this->m_handle.vtbl_ptr),",
                        f"tobj_dup(this->m_handle.data_ptr),",
                    )
        # static cast to root
        with target.indented(
            f"operator ::taihe::data_view() const& {{",
            f"}}",
        ):
            target.writelns(
                f"return ::taihe::data_view(this->m_handle.data_ptr);",
            )
        with target.indented(
            f"operator ::taihe::data_holder() const& {{",
            f"}}",
        ):
            target.writelns(
                f"return ::taihe::data_holder(tobj_dup(this->m_handle.data_ptr));",
            )

    def gen_iface_view_dynamic_cast(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        # dynamic cast from root
        with target.indented(
            f"explicit {iface_cpp_info.weak_name}(::taihe::data_view other) : {iface_cpp_info.weak_name}({{",
            f"}}) {{}}",
        ):
            target.writelns(
                f"{iface_abi_info.dynamic_cast}(other.data_ptr->rtti_ptr),",
                f"other.data_ptr,",
            )

    def gen_iface_virtual_type_decl(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        target.writelns(
            f"struct virtual_type;",
        )

    def gen_iface_methods_impl_decl(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        target.writelns(
            f"template<typename Impl>",
            f"struct methods_impl;",
        )

    def gen_iface_ftbl_decl(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        target.writelns(
            f"template<typename Impl>",
            f"static const {iface_abi_info.ftable} ftbl_impl;",
        )

    def gen_iface_vtbl_impl(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        target.writelns(
            f"template<typename Impl>",
        )
        with target.indented(
            f"static constexpr {iface_abi_info.vtable} vtbl_impl = {{",
            f"}};",
        ):
            for ancestor_info in iface_abi_info.ancestor_list:
                ancestor_cpp_info = IfaceCppInfo.get(self.am, ancestor_info.iface)
                target.writelns(
                    f".{ancestor_info.ftbl_ptr} = &{ancestor_cpp_info.full_weak_name}::template ftbl_impl<Impl>,",
                )

    def gen_iface_idmap_impl(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        target.writelns(
            f"template<typename Impl>",
        )
        with target.indented(
            f"static constexpr struct IdMapItem idmap_impl[{len(iface_abi_info.ancestor_dict)}] = {{",
            f"}};",
        ):
            for ancestor, info in iface_abi_info.ancestor_dict.items():
                ancestor_abi_info = IfaceAbiInfo.get(self.am, ancestor)
                target.writelns(
                    f"{{&{ancestor_abi_info.iid}, &vtbl_impl<Impl>.{info.ftbl_ptr}}},",
                )

    def gen_iface_infos(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        target.writelns(
            f"using vtable_type = {iface_abi_info.vtable};",
            f"using view_type = {iface_cpp_info.full_weak_name};",
            f"using holder_type = {iface_cpp_info.full_norm_name};",
            f"using abi_type = {iface_abi_info.mangled_name};",
        )

    def gen_iface_utils(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        with target.indented(
            f"bool is_error() const& {{",
            f"}}",
        ):
            target.writelns(
                f"return m_handle.vtbl_ptr == nullptr;",
            )
        with target.indented(
            f"virtual_type const& operator*() const& {{",
            f"}}",
        ):
            target.writelns(
                f"return *reinterpret_cast<virtual_type const*>(&m_handle);",
            )
        with target.indented(
            f"virtual_type const* operator->() const& {{",
            f"}}",
        ):
            target.writelns(
                f"return reinterpret_cast<virtual_type const*>(&m_handle);",
            )

    def gen_iface_holder_defn(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        with target.indented(
            f"namespace {iface_cpp_info.namespace} {{",
            f"}}",
            indent="",
        ):
            with target.indented(
                f"struct {iface_cpp_info.norm_name} : public {iface_cpp_info.full_weak_name} {{",
                f"}};",
            ):
                target.writelns(
                    f"static constexpr bool is_holder = true;",
                )
                target.writelns(
                    f"explicit {iface_cpp_info.norm_name}({iface_abi_info.as_owner} handle) : {iface_cpp_info.full_weak_name}(handle) {{}}",
                )
                with target.indented(
                    f"{iface_cpp_info.norm_name}& operator=({iface_cpp_info.full_norm_name} other) {{",
                    f"}}",
                ):
                    target.writelns(
                        f"::std::swap(this->m_handle, other.m_handle);",
                        f"return *this;",
                    )
                with target.indented(
                    f"~{iface_cpp_info.norm_name}() {{",
                    f"}}",
                ):
                    target.writelns(
                        f"tobj_drop(this->m_handle.data_ptr);",
                    )
                self.gen_iface_holder_static_cast(target)
                self.gen_iface_holder_dynamic_cast(target)

    def gen_iface_holder_static_cast(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        # copy/move constructors
        with target.indented(
            f"{iface_cpp_info.norm_name}({iface_cpp_info.full_weak_name} const& other) : {iface_cpp_info.norm_name}({{",
            f"}}) {{}}",
        ):
            target.writelns(
                f"other.m_handle.vtbl_ptr,",
                f"tobj_dup(other.m_handle.data_ptr),",
            )
        with target.indented(
            f"{iface_cpp_info.norm_name}({iface_cpp_info.full_norm_name} const& other) : {iface_cpp_info.norm_name}({{",
            f"}}) {{}}",
        ):
            target.writelns(
                f"other.m_handle.vtbl_ptr,",
                f"tobj_dup(other.m_handle.data_ptr),",
            )
        with target.indented(
            f"{iface_cpp_info.norm_name}({iface_cpp_info.full_norm_name}&& other) : {iface_cpp_info.norm_name}({{",
            f"}}) {{}}",
        ):
            target.writelns(
                f"other.m_handle.vtbl_ptr,",
                f"std::exchange(other.m_handle.data_ptr, nullptr),",
            )
        # copy/move to ancestors
        for ancestor, info in iface_abi_info.ancestor_dict.items():
            if ancestor is self.iface:
                continue
            ancestor_cpp_info = IfaceCppInfo.get(self.am, ancestor)
            with target.indented(
                f"operator {ancestor_cpp_info.full_weak_name}() const& {{",
                f"}}",
            ):
                with target.indented(
                    f"return {ancestor_cpp_info.full_weak_name}({{",
                    f"}});",
                ):
                    target.writelns(
                        f"{info.static_cast}(this->m_handle.vtbl_ptr),",
                        f"this->m_handle.data_ptr,",
                    )
            with target.indented(
                f"operator {ancestor_cpp_info.full_norm_name}() const& {{",
                f"}}",
            ):
                with target.indented(
                    f"return {ancestor_cpp_info.full_norm_name}({{",
                    f"}});",
                ):
                    target.writelns(
                        f"{info.static_cast}(this->m_handle.vtbl_ptr),",
                        f"tobj_dup(this->m_handle.data_ptr),",
                    )
            with target.indented(
                f"operator {ancestor_cpp_info.full_norm_name}() && {{",
                f"}}",
            ):
                with target.indented(
                    f"return {ancestor_cpp_info.full_norm_name}({{",
                    f"}});",
                ):
                    target.writelns(
                        f"{info.static_cast}(this->m_handle.vtbl_ptr),",
                        f"std::exchange(this->m_handle.data_ptr, nullptr),",
                    )
        # copy/move to root
        with target.indented(
            f"operator ::taihe::data_view() const& {{",
            f"}}",
        ):
            target.writelns(
                f"return ::taihe::data_view(this->m_handle.data_ptr);",
            )
        with target.indented(
            f"operator ::taihe::data_holder() const& {{",
            f"}}",
        ):
            target.writelns(
                f"return ::taihe::data_holder(tobj_dup(this->m_handle.data_ptr));",
            )
        with target.indented(
            f"operator ::taihe::data_holder() && {{",
            f"}}",
        ):
            target.writelns(
                f"return ::taihe::data_holder(std::exchange(this->m_handle.data_ptr, nullptr));",
            )

    def gen_iface_holder_dynamic_cast(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        # dynamic cast from root
        with target.indented(
            f"explicit {iface_cpp_info.norm_name}(::taihe::data_holder other) : {iface_cpp_info.norm_name}({{",
            f"}}) {{}}",
        ):
            target.writelns(
                f"{iface_abi_info.dynamic_cast}(other.data_ptr->rtti_ptr),",
                f"std::exchange(other.data_ptr, nullptr),",
            )

    def gen_iface_same(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        with target.indented(
            f"namespace {iface_cpp_info.weakspace} {{",
            f"}}",
            indent="",
        ):
            with target.indented(
                f"inline bool operator==({iface_cpp_info.as_param} lhs, {iface_cpp_info.as_param} rhs) {{",
                f"}}",
            ):
                target.writelns(
                    f"return ::taihe::data_view(lhs) == ::taihe::data_view(rhs);",
                )

    def gen_iface_hash(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        with target.indented(
            f"template<> struct ::std::hash<{iface_cpp_info.full_norm_name}> {{",
            f"}};",
        ):
            with target.indented(
                f"size_t operator()({iface_cpp_info.as_param} val) const {{",
                f"}}",
            ):
                target.writelns(
                    f"return ::std::hash<::taihe::data_holder>()(val);",
                )


class CppIfaceImplGenerator:
    def __init__(self, am: AnalysisManager, iface: IfaceDecl):
        self.am = am
        self.iface = iface

    def gen_iface_impl_file(self, om: OutputManager):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        target = CHeaderWriter(
            om,
            f"include/{iface_cpp_info.impl_header}",
            FileKind.CPP_HEADER,
        )
        with target:
            target.add_include(iface_cpp_info.defn_header)
            target.add_include(iface_abi_info.impl_header)
            for method in self.iface.methods:
                for param in method.params:
                    param_ty_cpp_info = TypeCppInfo.get(self.am, param.ty)
                    target.add_include(*param_ty_cpp_info.defn_headers)
                if isinstance(return_ty := method.return_ty, NonVoidType):
                    return_ty_cpp_info = TypeCppInfo.get(self.am, return_ty)
                    target.add_include(*return_ty_cpp_info.defn_headers)
            self.gen_iface_virtual_type_impl(target)
            self.gen_iface_methods_impl_impl(target)
            self.gen_iface_ftbl_impl(target)
            for ancestor, info in iface_abi_info.ancestor_dict.items():
                if ancestor is self.iface:
                    continue
                ancestor_cpp_info = IfaceCppInfo.get(self.am, ancestor)
                target.add_include(ancestor_cpp_info.impl_header)
            for method in self.iface.methods:
                for param in method.params:
                    return_ty_cpp_info = TypeCppInfo.get(self.am, param.ty)
                    target.add_include(*return_ty_cpp_info.impl_headers)
                if isinstance(return_ty := method.return_ty, NonVoidType):
                    return_ty_cpp_info = TypeCppInfo.get(self.am, return_ty)
                    target.add_include(*return_ty_cpp_info.impl_headers)

    def gen_iface_virtual_type_impl(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        with target.indented(
            f"struct {iface_cpp_info.full_weak_name}::virtual_type {{",
            f"}};",
        ):
            for method in self.iface.methods:
                self.gen_iface_virtual_type_method(target, method)

    def gen_iface_virtual_type_method(self, target: CHeaderWriter, method: IfaceMethodDecl):  # fmt: skip
        method_abi_info = IfaceMethodAbiInfo.get(self.am, method)
        method_cpp_info = IfaceMethodCppInfo.get(self.am, method)
        params_cpp = []
        args_abi = []
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        args_abi.append(f"*reinterpret_cast<{iface_abi_info.mangled_name} const*>(this)")  # fmt: skip
        for param in method.params:
            param_ty_cpp_info = TypeCppInfo.get(self.am, param.ty)
            params_cpp.append(f"{param_ty_cpp_info.as_param} {param.name}")
            args_abi.append(into_abi(param_ty_cpp_info.as_param, param.name))
        params_cpp_str = ", ".join(params_cpp)
        args_abi_str = ", ".join(args_abi)
        result_abi = f"{method_abi_info.wrap_name}({args_abi_str})"
        if isinstance(return_ty := method.return_ty, NonVoidType):
            return_ty_cpp_info = TypeCppInfo.get(self.am, return_ty)
            return_ty_cpp_name = return_ty_cpp_info.as_owner
            result_cpp = from_abi(return_ty_cpp_info.as_owner, result_abi)
        else:
            return_ty_cpp_name = "void"
            result_cpp = result_abi
        with target.indented(
            f"{return_ty_cpp_name} {method_cpp_info.call_name}({params_cpp_str}) const& {{",
            f"}}",
        ):
            target.writelns(
                f"return {result_cpp};",
            )

    def gen_iface_methods_impl_impl(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        target.writelns(
            f"template<typename Impl>",
        )
        with target.indented(
            f"struct {iface_cpp_info.full_weak_name}::methods_impl {{",
            f"}};",
        ):
            for method in self.iface.methods:
                self.gen_iface_methods_impl_method(target, method)

    def gen_iface_methods_impl_method(self, target: CHeaderWriter, method: IfaceMethodDecl):  # fmt: skip
        method_cpp_info = IfaceMethodCppInfo.get(self.am, method)
        params_abi = []
        args_cpp = []
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        params_abi.append(f"{iface_abi_info.as_param} tobj")
        for param in method.params:
            param_ty_abi_info = TypeAbiInfo.get(self.am, param.ty)
            param_ty_cpp_info = TypeCppInfo.get(self.am, param.ty)
            params_abi.append(f"{param_ty_abi_info.as_param} {param.name}")
            args_cpp.append(from_abi(param_ty_cpp_info.as_param, param.name))
        params_abi_str = ", ".join(params_abi)
        args_cpp_str = ", ".join(args_cpp)
        result_cpp = f"::taihe::cast_data_ptr<Impl>(tobj.data_ptr)->{method_cpp_info.impl_name}({args_cpp_str})"
        if isinstance(return_ty := method.return_ty, NonVoidType):
            return_ty_abi_info = TypeAbiInfo.get(self.am, return_ty)
            return_ty_cpp_info = TypeCppInfo.get(self.am, return_ty)
            return_ty_abi_name = return_ty_abi_info.as_owner
            result_abi = into_abi(return_ty_cpp_info.as_owner, result_cpp)
        else:
            return_ty_abi_name = "void"
            result_abi = result_cpp
        with target.indented(
            f"static {return_ty_abi_name} {method.name}({params_abi_str}) {{",
            f"}}",
        ):
            target.writelns(
                f"return {result_abi};",
            )

    def gen_iface_ftbl_impl(self, target: CHeaderWriter):
        iface_abi_info = IfaceAbiInfo.get(self.am, self.iface)
        iface_cpp_info = IfaceCppInfo.get(self.am, self.iface)
        target.writelns(
            f"template<typename Impl>",
        )
        with target.indented(
            f"constexpr {iface_abi_info.ftable} {iface_cpp_info.weakspace}::{iface_cpp_info.weak_name}::ftbl_impl = {{",
            f"}};",
        ):
            target.writelns(
                f".version = {iface_abi_info.version},",
            )
            with target.indented(
                f".methods = {{",
                f"}},",
            ):
                for method in self.iface.methods:
                    target.writelns(
                        f".{method.name} = &methods_impl<Impl>::{method.name},",
                    )
